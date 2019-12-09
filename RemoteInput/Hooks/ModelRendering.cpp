//
//  ModelRendering.cpp
//  RemoteInput
//
//  Created by Brandon on 2019-12-08.
//  Copyright © 2019 XIO. All rights reserved.
//

#include "ModelRendering.hpp"
#include "Hooks.hpp"
#include "ReflectionHook.hxx"
#include "ControlCenter.hxx"
#include <mutex>

template<typename T>
T ReadPointer(void* &ptr)
{
	T result = *static_cast<T*>(ptr);
	ptr = static_cast<T*>(ptr) + 1;
	return result;
}

template<typename T>
void WritePointer(void* &ptr, T result)
{
	*static_cast<T*>(ptr) = result;
	ptr = static_cast<T*>(ptr) + 1;
}

class Reflector
{
private:
	typedef std::unique_ptr<void, std::function<void(void* const)>> object;
	
	Reflection* env;
	void process_reflect_array_index(Reflection* eios, jarray array, void* &arguments, void* &response, int dimensions);
	
public:
	Reflector::object make_safe(void* object);
	
	Reflector(Reflection* env);
	~Reflector();
	
	Reflector(const Reflector &other) = delete;
	Reflector(Reflector &&other) = delete;
	Reflector& operator = (const Reflector &other) = delete;
	Reflector& operator = (Reflector &&other) = delete;
	
	Reflector::object GetObject(void* object, const Hook hook);
	void ReleaseObject(void* object);
	void ReleaseObjects(void** objects, std::size_t amount);
	bool GetBoolean(void* object, const Hook hook);
	char GetChar(void* object, const Hook hook);
	std::uint8_t GetByte(void* object, const Hook hook);
	std::int16_t GetShort(void* object, const Hook hook);
	std::int32_t GetInt(void* object, const Hook hook);
	std::int64_t GetLong(void* object, const Hook hook);
	float GetFloat(void* object, const Hook hook);
	double GetDouble(void* object, const Hook hook);
	std::string GetString(void* object, const Hook hook);
	Reflector::object GetArray(void* object, const Hook hook);
	std::int32_t GetArraySize(void* array);
	
	Reflector::object ArrayIndex(void* array, std::int32_t index);
	
	template<typename T>
	std::vector<T> GetArrayIndex(void* array, std::int32_t index, std::size_t length);
	
	template<typename T>
	std::vector<T> GetArrayIndex2D(void* array, std::size_t length, std::int32_t x, std::int32_t y);
	
	template<typename T>
	std::vector<T> GetArrayIndex3D(void* array, std::size_t length, std::int32_t x, std::int32_t y, std::int32_t z);
	
	template<typename T>
	std::vector<T> GetArrayIndex4D(void* array, std::size_t length, std::int32_t x, std::int32_t y, std::int32_t z, std::int32_t w);
};

Reflector* get_reflector(Reflection* reflection)
{
	for (std::size_t i = 0; i < 2048; ++i)
	{
		SINE[i] = static_cast<std::int32_t>(65536.0 * sin(static_cast<double>(i) * UNIT));
		COSINE[i] = static_cast<std::int32_t>(65536.0 * cos(static_cast<double>(i) * UNIT));
	}
	return new Reflector(reflection);
}

Reflector::Reflector(Reflection* env) : env(env)
{
}

Reflector::~Reflector()
{
}

Reflector::object Reflector::make_safe(void* object)
{
	auto deleter = [&](void* const ptr) {
		this->ReleaseObject(ptr);
	};

	return std::unique_ptr<void, decltype(deleter)>{static_cast<void*>(object), deleter};
}

Reflector::object Reflector::GetObject(void* object, const Hook hook)
{
	return make_safe(env->getField<jobject>(static_cast<jobject>(object), hook.hook()));
}

void Reflector::ReleaseObject(void* object)
{
	if (object)
	{
		env->getEnv()->DeleteGlobalRef(static_cast<jobject>(object));
	}
}

void Reflector::ReleaseObjects(void** objects, std::size_t amount)
{
	for (std::size_t i = 0; i < amount; ++i)
	{
		ReleaseObject(objects[i]);
	}
}

bool Reflector::GetBoolean(void* object, const Hook hook)
{
	return env->getPrimitive<jboolean>(static_cast<jobject>(object), hook.hook());
}

char Reflector::GetChar(void* object, const Hook hook)
{
	return env->getPrimitive<jchar>(static_cast<jobject>(object), hook.hook());
}

std::uint8_t Reflector::GetByte(void* object, const Hook hook)
{
	return env->getPrimitive<jbyte>(static_cast<jobject>(object), hook.hook());
}

std::int16_t Reflector::GetShort(void* object, const Hook hook)
{
	return env->getPrimitive<jshort>(static_cast<jobject>(object), hook.hook());
}

std::int32_t Reflector::GetInt(void* object, const Hook hook)
{
	return env->getPrimitive<jint>(static_cast<jobject>(object), hook.hook());
}

std::int64_t Reflector::GetLong(void* object, const Hook hook)
{
	return env->getPrimitive<jlong>(static_cast<jobject>(object), hook.hook());
}

float Reflector::GetFloat(void* object, const Hook hook)
{
	return env->getPrimitive<jfloat>(static_cast<jobject>(object), hook.hook());
}

double Reflector::GetDouble(void* object, const Hook hook)
{
	return env->getPrimitive<jdouble>(static_cast<jobject>(object), hook.hook());
}

std::string Reflector::GetString(void* object, const Hook hook)
{
	return env->getField<std::string>(static_cast<jobject>(object), hook.hook());
}

Reflector::object Reflector::GetArray(void* object, const Hook hook)
{
	return make_safe(env->getField<jobject>(static_cast<jobject>(object), hook.hook()));
}

std::int32_t Reflector::GetArraySize(void* array)
{
	return env->getEnv()->GetArrayLength(static_cast<jarray>(array));
}

Reflector::object Reflector::ArrayIndex(void* array, std::int32_t index)
{
	return make_safe(GetArrayIndex<jobject>(array, index, 1)[0]);
}

void Reflector::process_reflect_array_index(Reflection* eios, jarray array, void* &arguments, void* &response, int dimensions)
{
	auto write_result = [eios](jarray array, ReflectionArrayType type, jsize index, jsize length, void* &response) -> void {
		if (!array)
		{
			return WritePointer(response, nullptr);
		}

		//Maybe better to use GetPrimitiveArrayCritical + memcpy
		switch (type) {
			case ReflectionArrayType::CHAR:
			{
				eios->getEnv()->GetCharArrayRegion(static_cast<jcharArray>(array), index, length, static_cast<jchar*>(response));
			}
				break;

			case ReflectionArrayType::BYTE:
			{
				eios->getEnv()->GetByteArrayRegion(static_cast<jbyteArray>(array), index, length, static_cast<jbyte*>(response));
			}
				break;

			case ReflectionArrayType::BOOL:
			{
				eios->getEnv()->GetBooleanArrayRegion(static_cast<jbooleanArray>(array), index, length, static_cast<jboolean*>(response));
			}
				break;

			case ReflectionArrayType::SHORT:
			{
				eios->getEnv()->GetShortArrayRegion(static_cast<jshortArray>(array), index, length, static_cast<jshort*>(response));
			}
				break;

			case ReflectionArrayType::INT:
			{
				eios->getEnv()->GetIntArrayRegion(static_cast<jintArray>(array), index, length, static_cast<jint*>(response));
			}
				break;

			case ReflectionArrayType::LONG:
			{
				eios->getEnv()->GetLongArrayRegion(static_cast<jlongArray>(array), index, length, static_cast<jlong*>(response));
			}
				break;

			case ReflectionArrayType::FLOAT:
			{
				eios->getEnv()->GetFloatArrayRegion(static_cast<jfloatArray>(array), index, length, static_cast<jfloat*>(response));
			}
				break;

			case ReflectionArrayType::DOUBLE:
			{
				eios->getEnv()->GetDoubleArrayRegion(static_cast<jdoubleArray>(array), index, length, static_cast<jdouble*>(response));
			}
				break;

			case ReflectionArrayType::OBJECT:
			{
				if (length > 1)
				{
					for (jsize i = 0; i < length; ++i)
					{
						auto result = eios->getEnv()->GetObjectArrayElement(static_cast<jobjectArray>(array), index + i);
						WritePointer(response, result ? eios->getEnv()->NewGlobalRef(result) : nullptr);
					}
				}
				else
				{
					auto result = eios->getEnv()->GetObjectArrayElement(static_cast<jobjectArray>(array), index);
					WritePointer(response, result ? eios->getEnv()->NewGlobalRef(result) : nullptr);
				}
			}
				break;
		}
	};

	if (dimensions == 1)
	{
		ReflectionArrayType type = ReadPointer<ReflectionArrayType>(arguments);
		jsize index = ReadPointer<jsize>(arguments);
		jsize length = ReadPointer<jsize>(arguments);
		write_result(array, type, index, length, response);
		return;
	}

	ReflectionArrayType type = ReadPointer<ReflectionArrayType>(arguments);
	for (int i = 0; i < dimensions - 1; ++i)
	{
		jsize index = ReadPointer<jsize>(arguments);

		if (array)
		{
			array = static_cast<jarray>(eios->getEnv()->GetObjectArrayElement(static_cast<jobjectArray>(array), index));
		}
	}

	jsize index = ReadPointer<jsize>(arguments);
	jsize length = ReadPointer<jsize>(arguments);
	write_result(array, type, index, length, response);
}

template<typename T>
ReflectionArrayType GetArrayType()
{
	ReflectionArrayType type = ReflectionArrayType::OBJECT;
	if (std::is_same<T, char>::value || std::is_same<T, jchar>::value)
	{
		type = ReflectionArrayType::CHAR;
	}
	
	if (std::is_same<T, std::uint8_t>::value || std::is_same<T, jbyte>::value)
	{
		type = ReflectionArrayType::BYTE;
	}
	
	if (std::is_same<T, bool>::value || std::is_same<T, jboolean>::value)
	{
		type = ReflectionArrayType::BOOL;
	}
	
	if (std::is_same<T, std::int16_t>::value || std::is_same<T, jshort>::value)
	{
		type = ReflectionArrayType::SHORT;
	}
	
	if (std::is_same<T, std::int32_t>::value || std::is_same<T, jint>::value)
	{
		type = ReflectionArrayType::INT;
	}
	
	if (std::is_same<T, std::int64_t>::value || std::is_same<T, jlong>::value)
	{
		type = ReflectionArrayType::LONG;
	}
	
	if (std::is_same<T, float>::value || std::is_same<T, jfloat>::value)
	{
		type = ReflectionArrayType::FLOAT;
	}
	
	if (std::is_same<T, double>::value || std::is_same<T, jdouble>::value)
	{
		type = ReflectionArrayType::DOUBLE;
	}
	
	if (std::is_same<T, void*>::value || std::is_same<T, jobject>::value)
	{
		type = ReflectionArrayType::OBJECT;
	}
	return type;
}

template<typename T>
std::vector<T> Reflector::GetArrayIndex(void* array, std::int32_t index, std::size_t length)
{
	std::vector<std::uint8_t> args(1024);
	void* arguments = &args[0];
	
	WritePointer(arguments, GetArrayType<T>());
	WritePointer(arguments, static_cast<jsize>(index));
	WritePointer(arguments, static_cast<jsize>(length));
	
	std::vector<T> buffer(length);
	arguments = &args[0];
	void* response = &buffer[0];
	process_reflect_array_index(env, static_cast<jarray>(array), arguments, response, 1);
	return buffer;
}

template<typename T>
std::vector<T> Reflector::GetArrayIndex2D(void* array, std::size_t length, std::int32_t x, std::int32_t y)
{
	std::vector<std::uint8_t> args(1024);
	void* arguments = &args[0];
	
	WritePointer(arguments, GetArrayType<T>());
	WritePointer(arguments, static_cast<jsize>(x));
	WritePointer(arguments, static_cast<jsize>(y));
	WritePointer(arguments, static_cast<jsize>(length));
	
	std::vector<T> buffer(length);
	arguments = &args[0];
	void* response = &buffer[0];
	process_reflect_array_index(env, static_cast<jarray>(array), arguments, response, 2);
	return buffer;
}

template<typename T>
std::vector<T> Reflector::GetArrayIndex3D(void* array, std::size_t length, std::int32_t x, std::int32_t y, std::int32_t z)
{
	std::vector<std::uint8_t> args(1024);
	void* arguments = &args[0];
	
	WritePointer(arguments, GetArrayType<T>());
	WritePointer(arguments, static_cast<jsize>(x));
	WritePointer(arguments, static_cast<jsize>(y));
	WritePointer(arguments, static_cast<jsize>(z));
	WritePointer(arguments, static_cast<jsize>(length));
	
	std::vector<T> buffer(length);
	arguments = &args[0];
	void* response = &buffer[0];
	process_reflect_array_index(env, static_cast<jarray>(array), arguments, response, 3);
	return buffer;
}

template<typename T>
std::vector<T> Reflector::GetArrayIndex4D(void* array, std::size_t length, std::int32_t x, std::int32_t y, std::int32_t z, std::int32_t w)
{
	std::vector<std::uint8_t> args(1024);
	void* arguments = &args[0];
	
	WritePointer(arguments, GetArrayType<T>());
	WritePointer(arguments, static_cast<jsize>(x));
	WritePointer(arguments, static_cast<jsize>(y));
	WritePointer(arguments, static_cast<jsize>(z));
	WritePointer(arguments, static_cast<jsize>(w));
	WritePointer(arguments, static_cast<jsize>(length));
	
	std::vector<T> buffer(length);
	arguments = &args[0];
	void* response = &buffer[0];
	process_reflect_array_index(env, static_cast<jarray>(array), arguments, response, 4);
	return buffer;
}




// MARK: - MODELS

Point Point::rotate(std::int32_t orientation)
{
	return Point(((x * COSINE[orientation]) + (z * SINE[orientation])) >> 16,
				 y,
				 ((z * COSINE[orientation]) - (x * SINE[orientation])) >> 16
	);
}

Polygon::Polygon(std::vector<std::int32_t> xpoints, std::vector<std::int32_t> ypoints, std::int32_t npoints)
{
	if (npoints > xpoints.size() || npoints > ypoints.size())
	{
		throw "npoints > xpoints.length || npoints > ypoints.length";
	}
	
	if (npoints < 0)
	{
		throw "npoints < 0";
	}
	
	this->npoints = npoints;
	this->xpoints = xpoints;
	this->ypoints = ypoints;
}

// MARK: - Client

std::uint8_t Client::getTileSettings(Reflector* eios, std::int32_t x, std::int32_t y, std::int32_t z)
{
	auto settings = eios->GetArray(nullptr, HOOK_CLIENT_TILESETTINGS);
	return eios->GetArrayIndex3D<std::uint8_t>(settings.get(), 1, x, y, z)[0];
}

std::int32_t Client::getTileHeights(Reflector* eios, std::int32_t x, std::int32_t y, std::int32_t z)
{
	auto settings = eios->GetArray(nullptr, HOOK_CLIENT_TILEHEIGHTS);
	return eios->GetArrayIndex3D<std::int32_t>(settings.get(), 1, x, y, z)[0];
}

std::int32_t Client::plane(Reflector* eios)
{
	return eios->GetInt(nullptr, HOOK_CLIENT_PLANE) * static_cast<std::int32_t>(HOOK_CLIENT_PLANE.multiplier);
}

std::int32_t Client::viewPortWidth(Reflector* eios)
{
	return eios->GetInt(nullptr, HOOK_CLIENT_VIEWPORTWIDTH) * static_cast<std::int32_t>(HOOK_CLIENT_VIEWPORTWIDTH.multiplier);
}

std::int32_t Client::viewPortHeight(Reflector* eios)
{
	return eios->GetInt(nullptr, HOOK_CLIENT_VIEWPORTHEIGHT) * static_cast<std::int32_t>(HOOK_CLIENT_VIEWPORTHEIGHT.multiplier);
}

std::int32_t Client::viewPortScale(Reflector* eios)
{
	return eios->GetInt(nullptr, HOOK_CLIENT_VIEWPORTSCALE) * static_cast<std::int32_t>(HOOK_CLIENT_VIEWPORTSCALE.multiplier);
}

// MARK: - Camera

std::int32_t Camera::getX(Reflector* eios)
{
	return eios->GetInt(nullptr, HOOK_CLIENT_CAMERAX) * static_cast<std::int32_t>(HOOK_CLIENT_CAMERAX.multiplier);
}

std::int32_t Camera::getY(Reflector* eios)
{
	return eios->GetInt(nullptr, HOOK_CLIENT_CAMERAY) * static_cast<std::int32_t>(HOOK_CLIENT_CAMERAY.multiplier);
}

std::int32_t Camera::getZ(Reflector* eios)
{
	return eios->GetInt(nullptr, HOOK_CLIENT_CAMERAZ) * static_cast<std::int32_t>(HOOK_CLIENT_CAMERAZ.multiplier);
}

std::int32_t Camera::pitch(Reflector* eios)
{
	return eios->GetInt(nullptr, HOOK_CLIENT_CAMERAPITCH) * static_cast<std::int32_t>(HOOK_CLIENT_CAMERAPITCH.multiplier);
}

std::int32_t Camera::yaw(Reflector* eios)
{
	return eios->GetInt(nullptr, HOOK_CLIENT_CAMERAYAW) * static_cast<std::int32_t>(HOOK_CLIENT_CAMERAYAW.multiplier);
}

// MARK: - Projector

std::int32_t Projector::getTileHeight(Reflector* eios, std::int32_t x, std::int32_t y, std::int32_t plane)
{
	std::int32_t xSize = x >> 7;
	std::int32_t ySize = y >> 7;

	if (xSize >= 0 && ySize >= 0 && xSize < 104 && ySize < 104)
	{
		if (plane < 3 && (Client::getTileSettings(eios, 1, xSize, ySize) & 2) == 2)
		{
			++plane;
		}
		
		std::int32_t var1 = x & 127;
		std::int32_t var2 = y & 127;
		
		std::int32_t var3 = ((var1 * Client::getTileHeights(eios, plane, xSize + 1, ySize)) + ((128 - var1) * Client::getTileHeights(eios, plane, xSize, ySize))) >> 7;
		
		std::int32_t var4 = ((Client::getTileHeights(eios, plane, xSize, ySize + 1) * (128 - var1)) + (var1 * Client::getTileHeights(eios, plane, xSize + 1, ySize + 1))) >> 7;
		
		return (((128 - var2) * var3) + (var2 * var4)) >> 7;
	}

	return 0;
}

Point Projector::toScreen(Reflector* eios, Point p, std::int32_t localX, std::int32_t localY, std::int32_t plane)
{
	std::int32_t x = localX - p.x;
	std::int32_t y = localY - p.z;

	if (x >= 128 && y >= 128 && x <= 13056 && y <= 13056)
	{
		std::int32_t z = getTileHeight(eios, x, y, Client::plane(eios)) - plane;
		x -= Camera::getX(eios);
		y -= Camera::getY(eios);
		z -= Camera::getZ(eios);

		int cameraPitch = Camera::pitch(eios);
		int cameraYaw = Camera::yaw(eios);

		int sinCurveY = SINE[cameraPitch];
		int cosCurveY = COSINE[cameraPitch];
		int sinCurveX = SINE[cameraYaw];
		int cosCurveX = COSINE[cameraYaw];
		int calculation = ((sinCurveX * y) + (cosCurveX * x)) >> 16;
		y = ((y * cosCurveX) - (x * sinCurveX)) >> 16;
		x = calculation;
		calculation = ((cosCurveY * z) - (sinCurveY * y)) >> 16;
		y = ((sinCurveY * z) + (cosCurveY * y)) >> 16;
		z = calculation;

		if (y >= 50)
		{
			int viewWidth = Client::viewPortWidth(eios);
			int viewHeight = Client::viewPortHeight(eios);
			int screenX = (x * Client::viewPortScale(eios) / y) + (viewWidth / 2);
			int screenY = (z * Client::viewPortScale(eios) / y) + (viewHeight / 2);
			
			if (screenX < 0 || screenX > viewWidth || screenY < 0 || screenY > viewHeight)
			{
				return Point(-1, -1, 0);
			}
			else
			{
				return Point(screenX, screenY, 0);
			}
		}
	}

	return Point(-1, -1, 0);

}

// MARK: - Model
Model::Model(Reflector* eios, object&& ref) : eios(eios), ref(std::move(ref))
{
	if (this->ref)
	{
		this->vertices = this->getVertices();
		this->triangles = this->getTriangles();
	}
}

Model::Model(Model&& other) : ref(std::move(other.ref)), eios(other.eios), vertices(std::move(other.vertices)), triangles(std::move(other.triangles))
{
}

Model& Model::operator = (Model&& other)
{
	ref = std::move(other.ref);
	eios = other.eios;
	vertices = std::move(other.vertices);
	triangles = std::move(other.triangles);
	return *this;
}

Model::operator bool() const
{
	return ref.get() != nullptr;
}

void Model::scale(std::int32_t x, std::int32_t y, std::int32_t z)
{
	for (std::size_t i = 0; i < this->vertices.size(); ++i)
	{
		Point p = this->vertices[i];
		int px = p.x * x / 128;
		int py = p.y * x / 128;
		int pz = p.z * x / 128;
		this->vertices[i] = Point(px, py, pz);
	}
}

std::vector<Point> Model::getVertices()
{
	std::vector<Point> result;
	auto verticesX = eios->GetArray(ref.get(), HOOK_MODEL_VERTICESX);
	auto verticesY = eios->GetArray(ref.get(), HOOK_MODEL_VERTICESY);
	auto verticesZ = eios->GetArray(ref.get(), HOOK_MODEL_VERTICESZ);
	
	std::size_t size = eios->GetArraySize(verticesX.get());
	result.reserve(size);
	
	auto x = eios->GetArrayIndex<std::int32_t>(verticesX.get(), 0, size);
	auto y = eios->GetArrayIndex<std::int32_t>(verticesY.get(), 0, size);
	auto z = eios->GetArrayIndex<std::int32_t>(verticesZ.get(), 0, size);
	
	for (std::size_t i = 0; i < size; ++i)
	{
		result.push_back(Point(x[i], y[i], z[i]));
	}
	return result;
}

std::vector<Triangle> Model::getTriangles()
{
	std::vector<Triangle> result;
	auto indicesX = eios->GetArray(ref.get(), HOOK_MODEL_INDICESX);
	auto indicesY = eios->GetArray(ref.get(), HOOK_MODEL_INDICESY);
	auto indicesZ = eios->GetArray(ref.get(), HOOK_MODEL_INDICESZ);
	
	std::int32_t size = eios->GetArraySize(indicesX.get());
	result.reserve(size);
	
	auto x = eios->GetArrayIndex<std::int32_t>(indicesX.get(), 0, size);
	auto y = eios->GetArrayIndex<std::int32_t>(indicesY.get(), 0, size);
	auto z = eios->GetArrayIndex<std::int32_t>(indicesZ.get(), 0, size);
	
	for (std::int32_t i = 0; i < size; ++i)
	{
		Point a = this->vertices[x[i]];
		Point b = this->vertices[y[i]];
		Point c = this->vertices[z[i]];
		result.push_back(Triangle(a, b, c));
	}
	return result;
}

std::vector<Triangle> Model::getRotatedTriangles(std::int32_t orientation)
{
	for (auto triangle : this->triangles)
	{
		triangle.a = triangle.a.rotate(orientation);
		triangle.b = triangle.b.rotate(orientation);
		triangle.c = triangle.c.rotate(orientation);
	}
	
	return triangles;
}

// MARK: - Node
Node::Node(Reflector* eios, object&& ref) : eios(eios), ref(std::move(ref)) {}
Node::Node(Node&& other) : ref(std::move(other.ref)), eios(other.eios) {}
Node& Node::operator = (Node&& other)
{
	ref = std::move(other.ref);
	eios = other.eios;
	return *this;
}

Node::operator bool() const
{
	return (bool)ref;
}

std::int64_t Node::uid()
{
	return eios->GetLong(ref.get(), HOOK_NODE_UID) * HOOK_NODE_UID.multiplier;
}

Node Node::next()
{
	auto node = eios->GetObject(ref.get(), HOOK_NODE_NEXT);
	return Node(eios, std::move(node));
}

// MARK: - HashTable

HashTable::HashTable(Reflector* eios, object&& ref) : eios(eios), ref(std::move(ref)) {}

HashTable::operator bool() const
{
	return (bool)ref;
}

std::int32_t HashTable::size()
{
	return eios->GetInt(ref.get(), HOOK_HASHTABLE_CAPACITY) * static_cast<std::int32_t>(HOOK_HASHTABLE_CAPACITY.multiplier);
}

std::vector<Node> HashTable::buckets()
{
	std::vector<Node> result;
	auto buckets = eios->GetArray(ref.get(), HOOK_HASHTABLE_CACHE_BUCKETS);
	std::int32_t size = eios->GetArraySize(buckets.get());
	result.reserve(size);
	
	auto buckets_array = eios->GetArrayIndex<jobject>(buckets.get(), 0, size);
	
	for (std::int32_t i = 0; i < size; ++i)
	{
		result.push_back(Node(eios, eios->make_safe(buckets_array[i])));
	}
	
	return result;
}

// MARK: - Cache

Cache::Cache(Reflector* eios, object&& ref) : eios(eios), ref(std::move(ref)) {}
Cache::Cache(Cache&& other) : ref(std::move(other.ref)), eios(other.eios) {}

Cache& Cache::operator = (Cache&& other)
{
	ref = std::move(other.ref);
	eios = other.eios;
	return *this;
}

Cache::operator bool() const
{
	return (bool)ref;
}

HashTable Cache::hash_table()
{
	auto hashtable = eios->GetObject(ref.get(), HOOK_CACHE_HASHTABLE);
	return HashTable(eios, std::move(hashtable));
}

// MARK: PlayerDefinition

PlayerDefinition::PlayerDefinition(Reflector* eios, object&& ref) : eios(eios), ref(std::move(ref)) {}
PlayerDefinition::PlayerDefinition(PlayerDefinition&& other) : ref(std::move(other.ref)), eios(other.eios) {}

PlayerDefinition& PlayerDefinition::operator = (PlayerDefinition&& other)
{
	ref = std::move(other.ref);
	eios = other.eios;
	return *this;
}

PlayerDefinition::operator bool() const
{
	return (bool)ref;
}

std::int32_t PlayerDefinition::id()
{
	return eios->GetInt(ref.get(), HOOK_PLAYERDEFINITION_ID) * static_cast<std::int32_t>(HOOK_PLAYERDEFINITION_ID.multiplier);
}

std::int64_t PlayerDefinition::model_id()
{
	return eios->GetLong(ref.get(), HOOK_PLAYERDEFINITION_MODELID) * HOOK_PLAYERDEFINITION_MODELID.multiplier;
}

std::int64_t PlayerDefinition::animated_model_id()
{
	return eios->GetLong(ref.get(), HOOK_PLAYERDEFINITION_ANIMATEDMODELID) * HOOK_PLAYERDEFINITION_ANIMATEDMODELID.multiplier;
}

bool PlayerDefinition::is_female()
{
	return eios->GetBoolean(ref.get(), HOOK_PLAYERDEFINITION_ISFEMALE);
}

Cache PlayerDefinition::model_cache()
{
	auto cache = eios->GetObject(nullptr, HOOK_PLAYERDEFINITION_MODELCACHE);
	return Cache(eios, std::move(cache));
}

// MARK: - Player

Player::Player(Reflector* eios, object&& ref) : eios(eios), ref(std::move(ref)) {}
Player::Player(Player&& other) : ref(std::move(other.ref)), eios(other.eios) {}

Player& Player::operator = (Player&& other)
{
	ref = std::move(other.ref);
	eios = other.eios;
	return *this;
}

Player::operator bool() const
{
	return (bool)ref;
}

Player Player::localPlayer(Reflector* eios)
{
	auto player = eios->GetObject(nullptr, HOOK_CLIENT_LOCALPLAYER);
	return Player(eios, std::move(player));
}

std::vector<Player> Player::getAllPlayers(Reflector* eios)
{
	std::vector<Player> result;
	auto players = eios->GetArray(nullptr, HOOK_CLIENT_LOCALPLAYERS);
	std::int32_t player_count = eios->GetArraySize(players.get());
	
	auto array = eios->GetArrayIndex<jobject>(players.get(), 0, player_count);
	
	for (std::int32_t i = 0; i < player_count; ++i)
	{
		if (array[i])
		{
			result.push_back(Player(eios, eios->make_safe(array[i])));
		}
	}
	return result;
}

std::string Player::name()
{
	auto name_info = eios->GetObject(ref.get(), HOOK_PLAYER_NAME);
	if (name_info)
	{
		auto name = eios->GetString(name_info.get(), HOOK_NAMEINFO_NAME);
		return name;
	}
	return std::string();
}

std::int32_t Player::orientation()
{
	return eios->GetInt(ref.get(), HOOK_ENTITY_ORIENTATION) * static_cast<std::int32_t>(HOOK_ENTITY_ORIENTATION.multiplier);
}

std::int32_t Player::getX()
{
	return eios->GetInt(ref.get(), HOOK_ENTITY_LOCALX) * static_cast<std::int32_t>(HOOK_ENTITY_LOCALX.multiplier);
}

std::int32_t Player::getY()
{
	return eios->GetInt(ref.get(), HOOK_ENTITY_LOCALY) * static_cast<std::int32_t>(HOOK_ENTITY_LOCALY.multiplier);
}

PlayerDefinition Player::definition()
{
	auto definition = eios->GetObject(ref.get(), HOOK_PLAYER_DEFINITION);
	return PlayerDefinition(eios, std::move(definition));
}

Model Player::model()
{
	PlayerDefinition def = this->definition();
	if (!def)
	{
		return Model(eios, nullptr);
	}
	
	std::int64_t modelId = def.animated_model_id();
	
	Cache cache = def.model_cache();
	if (!cache)
	{
		return Model(eios, nullptr);
	}
	
	HashTable hashTable = cache.hash_table();
	if (!hashTable)
	{
		return Model(eios, nullptr);
	}
	
	std::vector<Node> buckets = hashTable.buckets();
	
	if (!buckets.empty())
	{
		std::int64_t index = (modelId & (hashTable.size() - 1));
		Node head = std::move(buckets[index]);
		Node current = head.next();
		while (head.uid() != current.uid())
		{
			std::int64_t uid = current.uid();
			if (uid == modelId)
			{
				return Model(eios, current.getRef());
			}

			if (uid == -1)
			{
				break;
			}

			current = current.next();
		}
	}
	return Model(eios, nullptr);
}





std::vector<Point> lineTo(std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2)
{
	std::int32_t Dx, Dy, CurrentX, CurrentY, Len, TwoDx, TwoDy, XInc, YInc;
	std::int32_t TwoDxAccumulatedError, TwoDyAccumulatedError;
	
	Len = 0;
	Dx = (X2-X1);
	Dy = (Y2-Y1);
	TwoDx = Dx + Dx;
	TwoDy = Dy + Dy;
	CurrentX = X1;
	CurrentY = Y1;
	XInc = 1;
	YInc = 1;
	
	if (Dx < 0)
	{
	  XInc = -1;
	  Dx = -Dx;
	  TwoDx = -TwoDx;
	}
	
	if (Dy < 0)
	{
	  YInc = -1;
	  Dy = -Dy;
	  TwoDy = -TwoDy;
	}
	
	std::vector<Point> result;
	result.resize(1);
	result[0] = Point(X1, Y1, 0);
	
	if ((Dx != 0) || (Dy != 0))
	{
	  if (Dy <= Dx)
	  {
		TwoDxAccumulatedError = 0;
		do
		{
			CurrentX += XInc;
			TwoDxAccumulatedError += TwoDy;
			
		  if (TwoDxAccumulatedError > Dx)
		  {
			  CurrentY += YInc;
			  TwoDxAccumulatedError -= TwoDx;
			  
		  }
			
			++Len;
			result.resize(Len + 1);
			result[Len] = Point(CurrentX, CurrentY, 0);
		}
		while (CurrentX != X2);
	  }
	  else
	  {
		TwoDyAccumulatedError = 0;
		  do
		  {
			  CurrentY += YInc;
			  TwoDyAccumulatedError += TwoDx;
			  
			  if (TwoDyAccumulatedError > Dy)
			  {
				  CurrentX += XInc;
				  TwoDyAccumulatedError -= TwoDy;
			  }
			  Len += 1;
			  result.resize(Len + 1);
			  result[Len] = Point(CurrentX, CurrentY, 0);
		  } while(CurrentY != Y2);
	  }
	}
	return result;
}

void draw_line(std::uint8_t* dest, Point a, Point b, std::int32_t width, std::int32_t height)
{
	auto points = lineTo(a.x, a.y, b.x, b.y);
	for (Point& p : points)
	{
		std::uint8_t* ptr = &dest[(p.x * width + p.y) * 4];
		*ptr++ = 0xFF;
		*ptr++ = 0xFF;
		*ptr++ = 0xFF;
		*ptr++ = 0xFF;
	}
}

void draw_polygon(std::uint8_t* dest, std::int32_t npoints, std::int32_t* xpoints, std::int32_t* ypoints, std::int32_t width, std::int32_t height)
{
	std::int32_t size = npoints - 2;
	if (size == -1)
	{
		return;
	}
	
	for (int i = 0; i <= size; ++i)
	{
		draw_line(dest, Point(xpoints[i], ypoints[i]), Point(xpoints[i + 1], ypoints[i + 1]), width, height);
	}
	
	draw_line(dest, Point(xpoints[size + 1], ypoints[size + 1]), Point(xpoints[0], ypoints[0]), width, height);
}

void draw_player(std::function<void(Polygon* p, std::int32_t width, std::int32_t height)> renderer)
{
	extern std::unique_ptr<ControlCenter> control_center;
	
	Reflector* eios = get_reflector(control_center->get_reflector());
	if (control_center->get_reflector()->Attach())
	{
		auto start = std::chrono::high_resolution_clock::now();
		std::vector<Player> players;
		players.push_back(Player::localPlayer(eios));
		
		for (auto& player : players)
		{
			if (player && !player.name().empty())
			{
				std::int32_t width = control_center->get_width();
				std::int32_t height = control_center->get_height();
				
				std::int32_t playerOrientation = player.orientation();
				std::int32_t orientation = ((playerOrientation & 0x3FFF) + 1024) % 2048;


				Model model = player.model();
				auto triangles = model.getRotatedTriangles(orientation);

				int localX = player.getX();
				int localY = player.getY();

				std::vector<Polygon> polygons;
				for (std::size_t i = 0; i < triangles.size(); ++i)
				{
					Point vx = triangles[i].a;
					Point vy = triangles[i].b;
					Point vz = triangles[i].c;

					Point a = Projector::toScreen(eios, vx, localX, localY, -vx.y);
					Point b = Projector::toScreen(eios, vy, localX, localY, -vy.y);
					Point c = Projector::toScreen(eios, vz, localX, localY, -vz.y);
					polygons.push_back(Polygon({a.x, b.x, c.x}, {a.y, b.y, c.y}, 3));
				}
				
				for (Polygon &p : polygons)
				{
					renderer(&p, width, height);
				}
			}
			
			auto end = std::chrono::high_resolution_clock::now();
			printf("TIME: %lld\n", std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count());
		}
		
		players.clear();
		
		control_center->get_reflector()->Detach();
	}
}
