#include <BNM/Defaults.hpp>
#include <Internals.hpp>


namespace BNM::Defaults::Internal {
    ClassType Void{}, Boolean{}, Byte{}, SByte{}, Int16{}, UInt16{}, Int32{}, UInt32{}, IntPtr{}, Int64{}, UInt64{}, Single{}, Double{}, String{}, Object{};
    ClassType Vector2{}, Vector3{}, Vector4{}, Color{}, Color32{}, Ray{}, Quaternion{}, Matrix3x3{}, Matrix4x4{}, RaycastHit{};
    ClassType UnityObject{}, MonoBehaviour{};
};

void BNM::Internal::LoadDefaults() {
    using namespace BNM::Defaults::Internal;

    // mscorlib
    auto image = il2cppMethods.il2cpp_get_corlib();
    Void = TryGetClassInImage(image, OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Void"));
    Boolean = TryGetClassInImage(image, OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Boolean"));
    Byte = TryGetClassInImage(image, OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Byte"));
    SByte = TryGetClassInImage(image, OBFUSCATE_BNM("System"), OBFUSCATE_BNM("SByte"));
    Int16 = TryGetClassInImage(image, OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Int16"));
    UInt16 = TryGetClassInImage(image, OBFUSCATE_BNM("System"), OBFUSCATE_BNM("UInt16"));
    Int32 = TryGetClassInImage(image, OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Int32"));
    UInt32 = TryGetClassInImage(image, OBFUSCATE_BNM("System"), OBFUSCATE_BNM("UInt32"));
    IntPtr = TryGetClassInImage(image, OBFUSCATE_BNM("System"), OBFUSCATE_BNM("IntPtr"));
    Int64 = TryGetClassInImage(image, OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Int64"));
    UInt64 = TryGetClassInImage(image, OBFUSCATE_BNM("System"), OBFUSCATE_BNM("UInt64"));
    Single = TryGetClassInImage(image, OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Single"));
    Double = TryGetClassInImage(image, OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Double"));
    String = TryGetClassInImage(image, OBFUSCATE_BNM("System"), OBFUSCATE_BNM("String"));
    Object = TryGetClassInImage(image, OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Object"));

    // Unity math
    image = TryGetImage(OBFUSCATE_BNM("UnityEngine.CoreModule"));
    Vector2 = TryGetClassInImage(image, OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Vector2"));
    Vector3 = TryGetClassInImage(image, OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Vector3"));
    Vector4 = TryGetClassInImage(image, OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Vector4"));
    Color = TryGetClassInImage(image, OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Color"));
    Color32 = TryGetClassInImage(image, OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Color32"));
    Ray = TryGetClassInImage(image, OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Ray"));
    Quaternion = TryGetClassInImage(image, OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Quaternion"));
    Matrix3x3 = TryGetClassInImage(image, OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Matrix3x3"));
    Matrix4x4 = TryGetClassInImage(image, OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Matrix4x4"));

    RaycastHit = TryGetClassInImage(TryGetImage(OBFUSCATE_BNM("UnityEngine.PhysicsModule.dll")), OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("RaycastHit"));

    // Unity
    UnityObject = TryGetClassInImage(image, OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Object"));
    MonoBehaviour = TryGetClassInImage(image, OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("MonoBehaviour"));
}

BNM::Defaults::DefaultTypeRef::operator BNM::CompileTimeClass() const{ return {.reference = reference, ._autoFree = false, ._isReferenced = true}; }
BNM::Defaults::DefaultTypeRef::operator BNM::Class() const{ return reference ? *reference : nullptr; }
BNM::Class BNM::Defaults::DefaultTypeRef::ToClass() const { return reference ? *reference : nullptr; }