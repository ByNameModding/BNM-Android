#include <BNM/Defaults.hpp>
#include <Internals.hpp>

namespace BNM::Defaults::Internal {
    ClassType Void{}, Boolean{}, Byte{}, SByte{}, Int16{}, UInt16{}, Int32{}, UInt32{}, IntPtr{}, Int64{}, UInt64{}, Single{}, Double{}, String{}, Object{};
    ClassType Vector2{}, Vector3{}, Vector4{}, Color{}, Color32{}, Ray{}, Quaternion{}, Matrix3x3{}, Matrix4x4{}, RaycastHit{};
    ClassType UnityObject{}, MonoBehaviour{};
}

void BNM::Internal::LoadDefaults() {
    using namespace BNM::Defaults::Internal;

    // mscorlib
    auto image = il2cppMethods.il2cpp_get_corlib();
    auto SystemStr = BNM_OBFUSCATE_TMP("System");
    auto ObjectStr = BNM_OBFUSCATE_TMP("Object");
    Void = TryGetClassInImage(image, SystemStr, BNM_OBFUSCATE_TMP("Void"));
    Boolean = TryGetClassInImage(image, SystemStr, BNM_OBFUSCATE_TMP("Boolean"));
    Byte = TryGetClassInImage(image, SystemStr, BNM_OBFUSCATE_TMP("Byte"));
    SByte = TryGetClassInImage(image, SystemStr, BNM_OBFUSCATE_TMP("SByte"));
    Int16 = TryGetClassInImage(image, SystemStr, BNM_OBFUSCATE_TMP("Int16"));
    UInt16 = TryGetClassInImage(image, SystemStr, BNM_OBFUSCATE_TMP("UInt16"));
    Int32 = TryGetClassInImage(image, SystemStr, BNM_OBFUSCATE_TMP("Int32"));
    UInt32 = TryGetClassInImage(image, SystemStr, BNM_OBFUSCATE_TMP("UInt32"));
    IntPtr = TryGetClassInImage(image, SystemStr, BNM_OBFUSCATE_TMP("IntPtr"));
    Int64 = TryGetClassInImage(image, SystemStr, BNM_OBFUSCATE_TMP("Int64"));
    UInt64 = TryGetClassInImage(image, SystemStr, BNM_OBFUSCATE_TMP("UInt64"));
    Single = TryGetClassInImage(image, SystemStr, BNM_OBFUSCATE_TMP("Single"));
    Double = TryGetClassInImage(image, SystemStr, BNM_OBFUSCATE_TMP("Double"));
    String = TryGetClassInImage(image, SystemStr, BNM_OBFUSCATE_TMP("String"));
    Object = TryGetClassInImage(image, SystemStr, ObjectStr);

    // Unity math
    auto UnityEngineStr = BNM_OBFUSCATE_TMP("UnityEngine");
    image = TryGetImage(BNM_OBFUSCATE_TMP("UnityEngine.CoreModule.dll"));
    Vector2 = TryGetClassInImage(image, UnityEngineStr, BNM_OBFUSCATE_TMP("Vector2"));
    Vector3 = TryGetClassInImage(image, UnityEngineStr, BNM_OBFUSCATE_TMP("Vector3"));
    Vector4 = TryGetClassInImage(image, UnityEngineStr, BNM_OBFUSCATE_TMP("Vector4"));
    Color = TryGetClassInImage(image, UnityEngineStr, BNM_OBFUSCATE_TMP("Color"));
    Color32 = TryGetClassInImage(image, UnityEngineStr, BNM_OBFUSCATE_TMP("Color32"));
    Ray = TryGetClassInImage(image, UnityEngineStr, BNM_OBFUSCATE_TMP("Ray"));
    Quaternion = TryGetClassInImage(image, UnityEngineStr, BNM_OBFUSCATE_TMP("Quaternion"));
    Matrix3x3 = TryGetClassInImage(image, UnityEngineStr, BNM_OBFUSCATE_TMP("Matrix3x3"));
    Matrix4x4 = TryGetClassInImage(image, UnityEngineStr, BNM_OBFUSCATE_TMP("Matrix4x4"));

    RaycastHit = TryGetClassInImage(TryGetImage(BNM_OBFUSCATE_TMP("UnityEngine.PhysicsModule.dll")), UnityEngineStr, BNM_OBFUSCATE_TMP("RaycastHit"));

    // Unity
    UnityObject = TryGetClassInImage(image, UnityEngineStr, ObjectStr);
    MonoBehaviour = TryGetClassInImage(image, UnityEngineStr, BNM_OBFUSCATE_TMP("MonoBehaviour"));
}

BNM::Defaults::DefaultTypeRef::operator BNM::CompileTimeClass() const{ return {.reference = reference, ._autoFree = false, ._isReferenced = true}; }
BNM::Defaults::DefaultTypeRef::operator BNM::Class() const{ return reference ? *reference : nullptr; }
BNM::Class BNM::Defaults::DefaultTypeRef::ToClass() const { return reference ? *reference : nullptr; }