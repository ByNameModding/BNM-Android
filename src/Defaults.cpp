#include <BNM/Defaults.hpp>
#include "Internals.hpp"


namespace BNM::Defaults::Internal {
    ClassType Void{}, Boolean{}, Byte{}, SByte{}, Int16{}, UInt16{}, Int32{}, UInt32{}, IntPtr{}, Int64{}, UInt64{}, Single{}, Double{}, String{}, Object{};
    ClassType Vector2{}, Vector3{}, Vector4{}, Color{}, Color32{}, Ray{}, RaycastHit{}, Quaternion{}, Matrix3x3{}, Matrix4x4{};
    ClassType UnityObject{}, MonoBehaviour{};
    ClassType Null{}; // Always null
};

void BNM::Internal::LoadDefaults() {
    using namespace BNM::Defaults::Internal;

    // mscorlib
    auto image = BNM::Image(il2cppMethods.il2cpp_get_corlib());
    Void = BNM::Class(OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Void"), image);
    Boolean = BNM::Class(OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Boolean"), image);
    Byte = BNM::Class(OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Byte"), image);
    SByte = BNM::Class(OBFUSCATE_BNM("System"), OBFUSCATE_BNM("SByte"), image);
    Int16 = BNM::Class(OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Int16"), image);
    UInt16 = BNM::Class(OBFUSCATE_BNM("System"), OBFUSCATE_BNM("UInt16"), image);
    Int32 = BNM::Class(OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Int32"), image);
    UInt32 = BNM::Class(OBFUSCATE_BNM("System"), OBFUSCATE_BNM("UInt32"), image);
    IntPtr = BNM::Class(OBFUSCATE_BNM("System"), OBFUSCATE_BNM("IntPtr"), image);
    Int64 = BNM::Class(OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Int64"), image);
    UInt64 = BNM::Class(OBFUSCATE_BNM("System"), OBFUSCATE_BNM("UInt64"), image);
    Single = BNM::Class(OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Single"), image);
    Double = BNM::Class(OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Double"), image);
    String = BNM::Class(OBFUSCATE_BNM("System"), OBFUSCATE_BNM("String"), image);
    Object = BNM::Class(OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Object"), image);

    // Unity math
    image = BNM::Image(OBFUSCATE_BNM("UnityEngine.CoreModule"));
    Vector2 = BNM::Class(OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Vector2"), image);
    Vector3 = BNM::Class(OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Vector3"), image);
    Vector4 = BNM::Class(OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Vector4"), image);
    Color = BNM::Class(OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Color"), image);
    Color32 = BNM::Class(OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Color32"), image);
    Ray = BNM::Class(OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Ray"), image);
    RaycastHit = BNM::Class(OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("RaycastHit"), BNM::Image(OBFUSCATE_BNM("UnityEngine.PhysicsModule.dll")));
    Quaternion = BNM::Class(OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Quaternion"), image);
    Matrix3x3 = BNM::Class(OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Matrix3x3"), image);
    Matrix4x4 = BNM::Class(OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Matrix4x4"), image);

    // Unity
    UnityObject = BNM::Class(OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Object"), image);
    MonoBehaviour = BNM::Class(OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("MonoBehaviour"), image);
}

BNM::Defaults::DefaultTypeRef::operator BNM::CompileTimeClass() const{ return {.reference = reference, ._autoFree = false, ._isReferenced = true}; }
BNM::Defaults::DefaultTypeRef::operator BNM::Class() const{ return *reference; }
BNM::Class BNM::Defaults::DefaultTypeRef::ToClass() const { return *reference; }