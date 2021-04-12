#pragma once

#include <stdint.h>
#include <stddef.h>
#include <cmath>

typedef int32_t il2cpp_hresult_t;

#define IL2CPP_LITTLE_ENDIAN 1
#define IL2CPP_BIG_ENDIAN 2
#define IL2CPP_BYTE_ORDER IL2CPP_LITTLE_ENDIAN

#define IL2CPP_ZERO_LEN_ARRAY 0

#define ALIGN_TYPE(val) __attribute__((aligned(val)))


#if !IL2CPP_DOTS_WITHOUT_DEBUGGER

struct Il2CppClass;
struct Il2CppType;
struct EventInfo;
struct MethodInfo;
struct FieldInfo;
struct PropertyInfo;

struct Il2CppAssembly;
struct Il2CppArray;
struct Il2CppDelegate;
struct Il2CppDomain;
struct Il2CppImage;
struct Il2CppException;
struct Il2CppProfiler;
struct Il2CppObject;
struct Il2CppReflectionMethod;
struct Il2CppReflectionType;
struct Il2CppString;
struct Il2CppThread;
struct Il2CppAsyncResult;

enum class Il2CppProfileFlags {
    IL2CPP_PROFILE_NONE = 0,
    IL2CPP_PROFILE_APPDOMAIN_EVENTS = 1 << 0,
    IL2CPP_PROFILE_ASSEMBLY_EVENTS = 1 << 1,
    IL2CPP_PROFILE_MODULE_EVENTS = 1 << 2,
    IL2CPP_PROFILE_CLASS_EVENTS = 1 << 3,
    IL2CPP_PROFILE_JIT_COMPILATION = 1 << 4,
    IL2CPP_PROFILE_INLINING = 1 << 5,
    IL2CPP_PROFILE_EXCEPTIONS = 1 << 6,
    IL2CPP_PROFILE_ALLOCATIONS = 1 << 7,
    IL2CPP_PROFILE_GC = 1 << 8,
    IL2CPP_PROFILE_THREADS = 1 << 9,
    IL2CPP_PROFILE_REMOTING = 1 << 10,
    IL2CPP_PROFILE_TRANSITIONS = 1 << 11,
    IL2CPP_PROFILE_ENTER_LEAVE = 1 << 12,
    IL2CPP_PROFILE_COVERAGE = 1 << 13,
    IL2CPP_PROFILE_INS_COVERAGE = 1 << 14,
    IL2CPP_PROFILE_STATISTICAL = 1 << 15,
    IL2CPP_PROFILE_METHOD_EVENTS = 1 << 16,
    IL2CPP_PROFILE_MONITOR_EVENTS = 1 << 17,
    IL2CPP_PROFILE_IOMAP_EVENTS = 1 << 18, /* this should likely be removed, too */
    IL2CPP_PROFILE_GC_MOVES = 1 << 19,
    IL2CPP_PROFILE_FILEIO = 1 << 20
};

enum class Il2CppProfileFileIOKind {
    IL2CPP_PROFILE_FILEIO_WRITE = 0,
    IL2CPP_PROFILE_FILEIO_READ
};


enum class Il2CppTypeEnum {
    IL2CPP_TYPE_END = 0x00, /* End of List */
    IL2CPP_TYPE_VOID = 0x01,
    IL2CPP_TYPE_BOOLEAN = 0x02,
    IL2CPP_TYPE_CHAR = 0x03,
    IL2CPP_TYPE_I1 = 0x04,
    IL2CPP_TYPE_U1 = 0x05,
    IL2CPP_TYPE_I2 = 0x06,
    IL2CPP_TYPE_U2 = 0x07,
    IL2CPP_TYPE_I4 = 0x08,
    IL2CPP_TYPE_U4 = 0x09,
    IL2CPP_TYPE_I8 = 0x0a,
    IL2CPP_TYPE_U8 = 0x0b,
    IL2CPP_TYPE_R4 = 0x0c,
    IL2CPP_TYPE_R8 = 0x0d,
    IL2CPP_TYPE_STRING = 0x0e,
    IL2CPP_TYPE_PTR = 0x0f,         /* arg: <type> token */
    IL2CPP_TYPE_BYREF = 0x10,       /* arg: <type> token */
    IL2CPP_TYPE_VALUETYPE = 0x11,   /* arg: <type> token */
    IL2CPP_TYPE_CLASS = 0x12,       /* arg: <type> token */
    IL2CPP_TYPE_VAR = 0x13,         /* Generic parameter in a generic type definition, represented as number (compressed unsigned integer) number */
    IL2CPP_TYPE_ARRAY = 0x14,       /* type, rank, boundsCount, bound1, loCount, lo1 */
    IL2CPP_TYPE_GENERICINST = 0x15, /* <type> <type-arg-count> <type-1> \x{2026} <type-n> */
    IL2CPP_TYPE_TYPEDBYREF = 0x16,
    IL2CPP_TYPE_I = 0x18,
    IL2CPP_TYPE_U = 0x19,
    IL2CPP_TYPE_FNPTR = 0x1b, /* arg: full method signature */
    IL2CPP_TYPE_OBJECT = 0x1c,
    IL2CPP_TYPE_SZARRAY = 0x1d,   /* 0-based one-dim-array */
    IL2CPP_TYPE_MVAR = 0x1e,      /* Generic parameter in a generic method definition, represented as number (compressed unsigned integer)  */
    IL2CPP_TYPE_CMOD_REQD = 0x1f, /* arg: typedef or typeref token */
    IL2CPP_TYPE_CMOD_OPT = 0x20,  /* optional arg: typedef or typref token */
    IL2CPP_TYPE_INTERNAL = 0x21,  /* CLR internal type */

    IL2CPP_TYPE_MODIFIER = 0x40, /* Or with the following types */
    IL2CPP_TYPE_SENTINEL = 0x41, /* Sentinel for varargs method signature */
    IL2CPP_TYPE_PINNED = 0x45,   /* Local var that points to pinned object */

    IL2CPP_TYPE_ENUM = 0x55 /* an enumeration */
};
#pragma once


typedef int32_t TypeIndex;
typedef int32_t TypeDefinitionIndex;
typedef int32_t FieldIndex;
typedef int32_t DefaultValueIndex;
typedef int32_t DefaultValueDataIndex;
typedef int32_t CustomAttributeIndex;
typedef int32_t ParameterIndex;
typedef int32_t MethodIndex;
typedef int32_t GenericMethodIndex;
typedef int32_t PropertyIndex;
typedef int32_t EventIndex;
typedef int32_t GenericContainerIndex;
typedef int32_t GenericParameterIndex;
typedef int16_t GenericParameterConstraintIndex;
typedef int32_t NestedTypeIndex;
typedef int32_t InterfacesIndex;
typedef int32_t VTableIndex;
typedef int32_t InterfaceOffsetIndex;
typedef int32_t RGCTXIndex;
typedef int32_t StringIndex;
typedef int32_t StringLiteralIndex;
typedef int32_t GenericInstIndex;
typedef int32_t ImageIndex;
typedef int32_t AssemblyIndex;
typedef int32_t InteropDataIndex;

const TypeIndex kTypeIndexInvalid = -1;
const TypeDefinitionIndex kTypeDefinitionIndexInvalid = -1;
const DefaultValueDataIndex kDefaultValueIndexNull = -1;
const EventIndex kEventIndexInvalid = -1;
const FieldIndex kFieldIndexInvalid = -1;
const MethodIndex kMethodIndexInvalid = -1;
const PropertyIndex kPropertyIndexInvalid = -1;
const GenericContainerIndex kGenericContainerIndexInvalid = -1;
const GenericParameterIndex kGenericParameterIndexInvalid = -1;
const RGCTXIndex kRGCTXIndexInvalid = -1;
const StringLiteralIndex kStringLiteralIndexInvalid = -1;
const InteropDataIndex kInteropDataIndexInvalid = -1;

// Encoded index (1 bit)
// MethodDef - 0
// MethodSpec - 1
// We use the top 3 bits to indicate what table to index into
// Type              Binary            Hex
// Il2CppClass          001               0x20000000
// Il2CppType        010               0x40000000
// MethodInfo        011               0x60000000
// FieldInfo         100               0x80000000
// StringLiteral     101               0xA0000000
// MethodRef         110               0xC0000000

typedef uint32_t EncodedMethodIndex;

enum class Il2CppMetadataUsage {
    kIl2CppMetadataUsageInvalid,
    kIl2CppMetadataUsageTypeInfo,
    kIl2CppMetadataUsageIl2CppType,
    kIl2CppMetadataUsageMethodDef,
    kIl2CppMetadataUsageFieldInfo,
    kIl2CppMetadataUsageStringLiteral,
    kIl2CppMetadataUsageMethodRef,
};

static inline Il2CppMetadataUsage GetEncodedIndexType(EncodedMethodIndex index) {
    return (Il2CppMetadataUsage) ((index & 0xE0000000) >> 29);
}

static inline uint32_t GetDecodedMethodIndex(EncodedMethodIndex index) {
    return index & 0x1FFFFFFFU;
}

struct Il2CppImage;
struct Il2CppType;
struct Il2CppTypeDefinitionMetadata;

union Il2CppRGCTXDefinitionData {
    int32_t rgctxDataDummy;
    MethodIndex methodIndex;
    TypeIndex typeIndex;
};

enum class Il2CppRGCTXDataType {
    IL2CPP_RGCTX_DATA_INVALID,
    IL2CPP_RGCTX_DATA_TYPE,
    IL2CPP_RGCTX_DATA_CLASS,
    IL2CPP_RGCTX_DATA_METHOD,
    IL2CPP_RGCTX_DATA_ARRAY,
};

struct Il2CppRGCTXDefinition {
    Il2CppRGCTXDataType type;
    Il2CppRGCTXDefinitionData data;
};

struct Il2CppInterfaceOffsetPair {
    TypeIndex interfaceTypeIndex;
    int32_t offset;
};

struct Il2CppTypeDefinition {
    StringIndex nameIndex;
    StringIndex namespaceIndex;
    CustomAttributeIndex customAttributeIndex;
    TypeIndex byvalTypeIndex;
    TypeIndex byrefTypeIndex;

    TypeIndex declaringTypeIndex;
    TypeIndex parentIndex;
    TypeIndex elementTypeIndex; // we can probably remove this one. Only used for enums

    RGCTXIndex rgctxStartIndex;
    int32_t rgctxCount;

    GenericContainerIndex genericContainerIndex;

    uint32_t flags;

    FieldIndex fieldStart;
    MethodIndex methodStart;
    EventIndex eventStart;
    PropertyIndex propertyStart;
    NestedTypeIndex nestedTypesStart;
    InterfacesIndex interfacesStart;
    VTableIndex vtableStart;
    InterfacesIndex interfaceOffsetsStart;

    uint16_t method_count;
    uint16_t property_count;
    uint16_t field_count;
    uint16_t event_count;
    uint16_t nested_type_count;
    uint16_t vtable_count;
    uint16_t interfaces_count;
    uint16_t interface_offsets_count;

    // bitfield to portably encode boolean values as single bits
    // 01 - valuetype;
    // 02 - enumtype;
    // 03 - has_finalize;
    // 04 - has_cctor;
    // 05 - is_blittable;
    // 06 - is_import_or_windows_runtime;
    // 07-10 - One of nine possible PackingSize values (0, 1, 2, 4, 8, 16, 32, 64, or 128)
    uint32_t bitfield;
    uint32_t token;
};

struct Il2CppFieldDefinition {
    StringIndex nameIndex;
    TypeIndex typeIndex;
    CustomAttributeIndex customAttributeIndex;
    uint32_t token;
};

struct Il2CppFieldDefaultValue {
    FieldIndex fieldIndex;
    TypeIndex typeIndex;
    DefaultValueDataIndex dataIndex;
};

struct Il2CppFieldMarshaledSize {
    FieldIndex fieldIndex;
    TypeIndex typeIndex;
    int32_t size;
};

struct Il2CppFieldRef {
    TypeIndex typeIndex;
    FieldIndex fieldIndex; // local offset into type fields
};

struct Il2CppParameterDefinition {
    StringIndex nameIndex;
    uint32_t token;
    CustomAttributeIndex customAttributeIndex;
    TypeIndex typeIndex;
};

struct Il2CppParameterDefaultValue {
    ParameterIndex parameterIndex;
    TypeIndex typeIndex;
    DefaultValueDataIndex dataIndex;
};

struct Il2CppMethodDefinition {
    StringIndex nameIndex;
    TypeDefinitionIndex declaringType;
    TypeIndex returnType;
    ParameterIndex parameterStart;
    CustomAttributeIndex customAttributeIndex;
    GenericContainerIndex genericContainerIndex;
    MethodIndex methodIndex;
    MethodIndex invokerIndex;
    MethodIndex reversePInvokeWrapperIndex;
    RGCTXIndex rgctxStartIndex;
    int32_t rgctxCount;
    uint32_t token;
    uint16_t flags;
    uint16_t iflags;
    uint16_t slot;
    uint16_t parameterCount;
};

struct Il2CppEventDefinition {
    StringIndex nameIndex;
    TypeIndex typeIndex;
    MethodIndex add;
    MethodIndex remove;
    MethodIndex raise;
    CustomAttributeIndex customAttributeIndex;
    uint32_t token;
};

struct Il2CppPropertyDefinition {
    StringIndex nameIndex;
    MethodIndex get;
    MethodIndex set;
    uint32_t attrs;
    CustomAttributeIndex customAttributeIndex;
    uint32_t token;
};

struct Il2CppMethodSpec {
    MethodIndex methodDefinitionIndex;
    GenericInstIndex classIndexIndex;
    GenericInstIndex methodIndexIndex;
};

struct Il2CppStringLiteral {
    uint32_t length;
    StringLiteralIndex dataIndex;
};

struct Il2CppGenericMethodIndices {
    MethodIndex methodIndex;
    MethodIndex invokerIndex;
};

struct Il2CppGenericMethodFunctionsDefinitions {
    GenericMethodIndex genericMethodIndex;
    Il2CppGenericMethodIndices indices;
};

const int kPublicKeyByteLength = 8;

struct Il2CppAssemblyName {
    StringIndex nameIndex;
    StringIndex cultureIndex;
    StringIndex hashValueIndex;
    StringIndex publicKeyIndex;
    uint32_t hash_alg;
    int32_t hash_len;
    uint32_t flags;
    int32_t major;
    int32_t minor;
    int32_t build;
    int32_t revision;
    uint8_t publicKeyToken[kPublicKeyByteLength];
};

struct Il2CppImageDefinition {
    StringIndex nameIndex;
    AssemblyIndex assemblyIndex;

    TypeDefinitionIndex typeStart;
    uint32_t typeCount;

    TypeDefinitionIndex exportedTypeStart;
    uint32_t exportedTypeCount;

    MethodIndex entryPointIndex;
    uint32_t token;
};

struct Il2CppAssembly {
    ImageIndex imageIndex;
    CustomAttributeIndex customAttributeIndex;
    int32_t referencedAssemblyStart;
    int32_t referencedAssemblyCount;
    Il2CppAssemblyName aname;
};

struct Il2CppMetadataUsageList {
    uint32_t start;
    uint32_t count;
};

struct Il2CppMetadataUsagePair {
    uint32_t destinationIndex;
    uint32_t encodedSourceIndex;
};

struct Il2CppCustomAttributeTypeRange {
    int32_t start;
    int32_t count;
};

struct Il2CppRange {
    int32_t start;
    int32_t length;
};

struct Il2CppWindowsRuntimeTypeNamePair {
    StringIndex nameIndex;
    TypeIndex typeIndex;
};

enum class Il2CppGCEvent {
    IL2CPP_GC_EVENT_START,
    IL2CPP_GC_EVENT_MARK_START,
    IL2CPP_GC_EVENT_MARK_END,
    IL2CPP_GC_EVENT_RECLAIM_START,
    IL2CPP_GC_EVENT_RECLAIM_END,
    IL2CPP_GC_EVENT_END,
    IL2CPP_GC_EVENT_PRE_STOP_WORLD,
    IL2CPP_GC_EVENT_POST_STOP_WORLD,
    IL2CPP_GC_EVENT_PRE_START_WORLD,
    IL2CPP_GC_EVENT_POST_START_WORLD
};

enum class Il2CppStat {
    IL2CPP_STAT_NEW_OBJECT_COUNT,
    IL2CPP_STAT_INITIALIZED_CLASS_COUNT,
    //IL2CPP_STAT_GENERIC_VTABLE_COUNT,
    //IL2CPP_STAT_USED_CLASS_COUNT,
    IL2CPP_STAT_METHOD_COUNT,
    //IL2CPP_STAT_CLASS_VTABLE_SIZE,
    IL2CPP_STAT_CLASS_STATIC_DATA_SIZE,
    IL2CPP_STAT_GENERIC_INSTANCE_COUNT,
    IL2CPP_STAT_GENERIC_CLASS_COUNT,
    IL2CPP_STAT_INFLATED_METHOD_COUNT,
    IL2CPP_STAT_INFLATED_TYPE_COUNT,
    //IL2CPP_STAT_DELEGATE_CREATIONS,
    //IL2CPP_STAT_MINOR_GC_COUNT,
    //IL2CPP_STAT_MAJOR_GC_COUNT,
    //IL2CPP_STAT_MINOR_GC_TIME_USECS,
    //IL2CPP_STAT_MAJOR_GC_TIME_USECS
};

enum class Il2CppRuntimeUnhandledExceptionPolicy {
    IL2CPP_UNHANDLED_POLICY_LEGACY,
    IL2CPP_UNHANDLED_POLICY_CURRENT
};

struct Il2CppStackFrameInfo {
    const MethodInfo *method;
};

typedef struct {
    void *(*malloc_func)(size_t size);

    void *(*aligned_malloc_func)(size_t size, size_t alignment);

    void (*free_func)(void *ptr);

    void (*aligned_free_func)(void *ptr);

    void *(*calloc_func)(size_t nmemb, size_t size);

    void *(*realloc_func)(void *ptr, size_t size);

    void *(*aligned_realloc_func)(void *ptr, size_t size, size_t alignment);
} Il2CppMemoryCallbacks;

typedef uint16_t Il2CppChar;

typedef char Il2CppNativeChar;
#define IL2CPP_NATIVE_STRING(str) str

typedef void (*il2cpp_register_object_callback)(Il2CppObject **arr, int size, void *userdata);

typedef void (*il2cpp_WorldChangedCallback)();

typedef void (*Il2CppFrameWalkFunc)(const Il2CppStackFrameInfo *info, void *user_data);

typedef void (*Il2CppProfileFunc)(Il2CppProfiler *prof);

typedef void (*Il2CppProfileMethodFunc)(Il2CppProfiler *prof, const MethodInfo *method);

typedef void (*Il2CppProfileAllocFunc)(Il2CppProfiler *prof, Il2CppObject *obj, Il2CppClass *klass);

typedef void (*Il2CppProfileGCFunc)(Il2CppProfiler *prof, Il2CppGCEvent event, int generation);

typedef void (*Il2CppProfileGCResizeFunc)(Il2CppProfiler *prof, int64_t new_size);

typedef void (*Il2CppProfileFileIOFunc)(Il2CppProfiler *prof, Il2CppProfileFileIOKind kind,
                                        int count);

typedef const Il2CppNativeChar *(*Il2CppSetFindPlugInCallback)(const Il2CppNativeChar *);

typedef void (*Il2CppLogCallback)(const char *);

struct Il2CppManagedMemorySnapshot;

typedef void (*Il2CppMethodPointer)();

typedef uintptr_t il2cpp_array_size_t;
#define ARRAY_LENGTH_AS_INT32(a) ((int32_t)a)

struct Il2CppClass;
struct MethodInfo;
struct Il2CppType;

struct Il2CppArrayType {
    const Il2CppType *etype;
    uint8_t rank;
    uint8_t numsizes;
    uint8_t numlobounds;
    int *sizes;
    int *lobounds;
};

struct Il2CppGenericInst {
    uint32_t type_argc;
    const Il2CppType **type_argv;
};

struct Il2CppGenericContext {
    /* The instantiation corresponding to the class generic parameters */
    const Il2CppGenericInst *class_inst;
    /* The instantiation corresponding to the method generic parameters */
    const Il2CppGenericInst *method_inst;
};

struct Il2CppGenericParameter {
    GenericContainerIndex ownerIndex; /* Type or method this parameter was defined in. */
    StringIndex nameIndex;
    GenericParameterConstraintIndex constraintsStart;
    int16_t constraintsCount;
    uint16_t num;
    uint16_t flags;
};

struct Il2CppGenericContainer {
    /* index of the generic type definition or the generic method definition corresponding to this container */
    int32_t ownerIndex; // either index into Il2CppClass metadata array or Il2CppMethodDefinition array
    int32_t type_argc;
    /* If true, we're a generic method, otherwise a generic type definition. */
    int32_t is_method;
    /* Our type parameters. */
    GenericParameterIndex genericParameterStart;
};

struct Il2CppGenericClass {
    TypeDefinitionIndex typeDefinitionIndex; /* the generic type definition */
    Il2CppGenericContext context;            /* a context that contains the type instantiation doesn't contain any method instantiation */
    Il2CppClass *cached_class;               /* if present, the Il2CppClass corresponding to the instantiation.  */
};

struct Il2CppGenericMethod {
    const MethodInfo *methodDefinition;
    Il2CppGenericContext context;
};

struct Il2CppType {
    union {
        // We have this dummy field first because pre C99 compilers (MSVC) can only initializer the first value in a union.
        void *dummy;
        TypeDefinitionIndex klassIndex; /* for VALUETYPE and CLASS */
        const Il2CppType *type;         /* for PTR and SZARRAY */
        Il2CppArrayType *array;         /* for ARRAY */
        //MonoMethodSignature *method;
        GenericParameterIndex genericParameterIndex; /* for VAR and MVAR */
        Il2CppGenericClass *generic_class;           /* for GENERICINST */
    } data;
    unsigned int attrs: 16; /* param attributes or field flags */
    Il2CppTypeEnum type: 8;
    unsigned int num_mods: 6; /* max 64 modifiers follow at the end */
    unsigned int byref: 1;
    unsigned int pinned: 1; /* valid when included in a local var signature */
    //MonoCustomMod modifiers [MONO_ZERO_LEN_ARRAY]; /* this may grow */
};

typedef enum {
    IL2CPP_CALL_DEFAULT,
    IL2CPP_CALL_C,
    IL2CPP_CALL_STDCALL,
    IL2CPP_CALL_THISCALL,
    IL2CPP_CALL_FASTCALL,
    IL2CPP_CALL_VARARG
} Il2CppCallConvention;

enum class Il2CppCharSet {
    CHARSET_ANSI,
    CHARSET_UNICODE
};


struct Il2CppType;
struct Il2CppObject;
struct Il2CppClass;
struct Il2CppGuid;
struct Il2CppImage;
struct Il2CppAssembly;
struct Il2CppAppDomain;
struct Il2CppAppDomainSetup;
struct Il2CppDelegate;
struct Il2CppAppContext;
struct Il2CppNameToTypeDefinitionIndexHashTable;

typedef void *(*InvokerMethod)(Il2CppMethodPointer, const MethodInfo *, void *, void **);

typedef void (*PInvokeMarshalToNativeFunc)(void *managedStructure, void *marshaledStructure);

typedef void (*PInvokeMarshalFromNativeFunc)(void *marshaledStructure, void *managedStructure);

typedef void (*PInvokeMarshalCleanupFunc)(void *marshaledStructure);

typedef struct Il2CppIUnknown *(*CreateCCWFunc)(Il2CppObject *obj);

struct Il2CppInteropData {
    Il2CppMethodPointer delegatePInvokeWrapperFunction;
    PInvokeMarshalToNativeFunc pinvokeMarshalToNativeFunction;
    PInvokeMarshalFromNativeFunc pinvokeMarshalFromNativeFunction;
    PInvokeMarshalCleanupFunc pinvokeMarshalCleanupFunction;
    CreateCCWFunc createCCWFunction;
    const Il2CppGuid *guid;
    const Il2CppType *type;
};

struct Il2CppRuntimeInterfaceOffsetPair {
    Il2CppClass *interfaceType;
    int32_t offset;
};

union Il2CppRGCTXData {
    void *rgctxDataDummy;
    const MethodInfo *method;
    const Il2CppType *type;
    Il2CppClass *klass;
};

struct VirtualInvokeData {
    Il2CppMethodPointer methodPtr;
    const MethodInfo *method;
};
struct FieldInfo
{
public:
    const char* name;
    const Il2CppType* type;
    Il2CppClass *parent;
    int32_t offset;
    uint32_t token;
};
struct Il2CppClass {
    const Il2CppImage *image;
    void *gc_desc;
    const char *name;
    const char *namespaze;
    const Il2CppType *byval_arg;
    const Il2CppType *this_arg;
    Il2CppClass *element_class;
    Il2CppClass *castClass;
    Il2CppClass *declaringType;
    Il2CppClass *parent;
    Il2CppGenericClass *generic_class;
    const Il2CppTypeDefinition *typeDefinition;
    const Il2CppInteropData *interopData;
    FieldInfo *fields;                                  // Initialized in SetupFields
    const EventInfo *events;                            // Initialized in SetupEvents
    const PropertyInfo *properties;                     // Initialized in SetupProperties
    const MethodInfo **methods;                         // Initialized in SetupMethods
    Il2CppClass **nestedTypes;                          // Initialized in SetupNestedTypes
    Il2CppClass **implementedInterfaces;                // Initialized in SetupInterfaces
    Il2CppRuntimeInterfaceOffsetPair *interfaceOffsets; // Initialized in Init
    void *static_fields;                                // Initialized in Init
    const Il2CppRGCTXData *rgctx_data;                  // Initialized in Init
    // used for fast parent checks
    Il2CppClass **typeHierarchy; // Initialized in SetupTypeHierachy
    // End initialization required fields

    uint32_t cctor_started;
    uint32_t cctor_finished;
    ALIGN_TYPE(8)
    uint64_t cctor_thread;

    // Remaining fields are always valid except where noted
    GenericContainerIndex genericContainerIndex;
    CustomAttributeIndex customAttributeIndex;
    uint32_t instance_size;
    uint32_t actualSize;
    uint32_t element_size;
    int32_t native_size;
    uint32_t static_fields_size;
    uint32_t thread_static_fields_size;
    int32_t thread_static_fields_offset;
    uint32_t flags;
    uint32_t token;

    uint16_t method_count; // lazily calculated for arrays, i.e. when rank > 0
    uint16_t property_count;
    uint16_t field_count;
    uint16_t event_count;
    uint16_t nested_type_count;
    uint16_t vtable_count; // lazily calculated for arrays, i.e. when rank > 0
    uint16_t interfaces_count;
    uint16_t interface_offsets_count; // lazily calculated for arrays, i.e. when rank > 0

    uint8_t typeHierarchyDepth; // Initialized in SetupTypeHierachy
    uint8_t genericRecursionDepth;
    uint8_t rank;
    uint8_t minimumAlignment;
    uint8_t packingSize;

    uint8_t valuetype: 1;
    uint8_t initialized: 1;
    uint8_t enumtype: 1;
    uint8_t is_generic: 1;
    uint8_t has_references: 1;
    uint8_t init_pending: 1;
    uint8_t size_inited: 1;
    uint8_t has_finalize: 1;
    uint8_t has_cctor: 1;
    uint8_t is_blittable: 1;
    uint8_t is_import_or_windows_runtime: 1;
    uint8_t is_vtable_initialized: 1;
    VirtualInvokeData vtable[IL2CPP_ZERO_LEN_ARRAY];
};

struct Il2CppImage {
    const char *name;
    const char *nameNoExt;
    AssemblyIndex assemblyIndex;

    TypeDefinitionIndex typeStart;
    uint32_t typeCount;

    TypeDefinitionIndex exportedTypeStart;
    uint32_t exportedTypeCount;

    MethodIndex entryPointIndex;

    mutable Il2CppNameToTypeDefinitionIndexHashTable *nameToClassHashTable;

    uint32_t token;
};

struct ParameterInfo {
    const char *name;
    int32_t position;
    uint32_t token;
    CustomAttributeIndex customAttributeIndex;
    const Il2CppType *parameter_type;
};

struct MethodInfo {
    Il2CppMethodPointer methodPointer;
    InvokerMethod invoker_method;
    const char *name;
    Il2CppClass *declaring_type;
    const Il2CppType *return_type;
    const ParameterInfo *parameters;

    union {
        const Il2CppRGCTXData *rgctx_data; /* is_inflated is true and is_generic is false, i.e. a generic instance method */
        const Il2CppMethodDefinition *methodDefinition;
    };

    /* note, when is_generic == true and is_inflated == true the method represents an uninflated generic method on an inflated type. */
    union {
        const Il2CppGenericMethod *genericMethod;       /* is_inflated is true */
        const Il2CppGenericContainer *genericContainer; /* is_inflated is false and is_generic is true */
    };

    CustomAttributeIndex customAttributeIndex;
    uint32_t token;
    uint16_t flags;
    uint16_t iflags;
    uint16_t slot;
    uint8_t parameters_count;
    uint8_t is_generic: 1;  /* true if method is a generic method definition */
    uint8_t is_inflated: 1; /* true if declaring_type is a generic instance or if method is a generic instance*/
};


#if defined(__cplusplus)
extern "C"
{
#endif // __cplusplus
// We cannot include il2cpp-api.h here because generated code contains il2cpp
// api declarations with mismatching parameter declarations (char* vs const char*).
// So we only declare il2cpp_gc_wbarrier_set_field here.
void il2cpp_gc_wbarrier_set_field(Il2CppObject *obj, void **targetAddress, void *object);
#if defined(__cplusplus)
}
#endif // __cplusplus

typedef struct Il2CppClass Il2CppClass;
typedef struct MethodInfo MethodInfo;
typedef struct PropertyInfo PropertyInfo;
typedef struct FieldInfo FieldInfo;
typedef struct EventInfo EventInfo;
typedef struct Il2CppType Il2CppType;
typedef struct Il2CppAssembly Il2CppAssembly;
typedef struct Il2CppException Il2CppException;
typedef struct Il2CppImage Il2CppImage;
typedef struct Il2CppDomain Il2CppDomain;
typedef struct Il2CppString Il2CppString;
typedef struct Il2CppReflectionMethod Il2CppReflectionMethod;
typedef struct Il2CppAsyncCall Il2CppAsyncCall;
typedef struct Il2CppIUnknown Il2CppIUnknown;
typedef struct Il2CppWaitHandle Il2CppWaitHandle;
typedef struct MonitorData MonitorData;

#ifdef __cplusplus
namespace il2cpp {
    namespace os {
        class FastMutex;

        class Thread;
    }
}
#endif //__cplusplus

typedef struct Il2CppReflectionAssembly Il2CppReflectionAssembly;

typedef Il2CppClass Il2CppVTable;
typedef struct Il2CppObject {
    union {
        Il2CppClass *klass;
        Il2CppVTable *vtable;
    };
    MonitorData *monitor;
} Il2CppObject;

typedef int32_t il2cpp_array_lower_bound_t;
#define IL2CPP_ARRAY_MAX_INDEX ((int32_t) 0x7fffffff)
#define IL2CPP_ARRAY_MAX_SIZE  ((uint32_t) 0xffffffff)

typedef struct Il2CppArrayBounds {
    il2cpp_array_size_t length;
    il2cpp_array_lower_bound_t lower_bound;
} Il2CppArrayBounds;

#if IL2CPP_COMPILER_MSVC
#pragma warning( push )
#pragma warning( disable : 4200 )
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winvalid-offsetof"
#endif

//Warning: Updates to this struct must also be made to IL2CPPArraySize C code
#ifdef __cplusplus
typedef struct Il2CppArray : public Il2CppObject {
#else
    typedef struct Il2CppArray
    {
        Il2CppObject obj;
#endif //__cplusplus
    /* bounds is NULL for szarrays */
    Il2CppArrayBounds *bounds;
    /* total number of elements of the array */
    il2cpp_array_size_t max_length;
} Il2CppArray;

#ifdef __cplusplus
typedef struct Il2CppArraySize : public Il2CppArray {
#else
    //mono code has no inheritance, so its members must be available from this type
    typedef struct Il2CppArraySize
    {
        Il2CppObject obj;
        Il2CppArrayBounds *bounds;
        il2cpp_array_size_t max_length;
#endif //__cplusplus
    ALIGN_TYPE(8) void *vector[IL2CPP_ZERO_LEN_ARRAY];
} Il2CppArraySize;

static const size_t kIl2CppSizeOfArray = (offsetof(Il2CppArraySize, vector));
static const size_t kIl2CppOffsetOfArrayBounds = (offsetof(Il2CppArray, bounds));
static const size_t kIl2CppOffsetOfArrayLength = (offsetof(Il2CppArray, max_length));


// System.String
typedef struct Il2CppString {
    Il2CppObject object;
    int32_t length;                             ///< Length of string *excluding* the trailing null (which is included in 'chars').
    Il2CppChar chars[IL2CPP_ZERO_LEN_ARRAY];
} Il2CppString;

#if IL2CPP_COMPILER_MSVC
#pragma warning( pop )
#elif defined(__clang__)
#pragma clang diagnostic pop
#endif

#define IL2CPP_OBJECT_SETREF(obj, fieldname, value) do {\
        il2cpp_gc_wbarrier_set_field((Il2CppObject *)(obj), (void**)&(obj)->fieldname, (value));\
    } while (0)

/* This should be used if 's' can reside on the heap */
#define IL2CPP_STRUCT_SETREF(s, field, value) do {\
        il2cpp_gc_wbarrier_set_field((Il2CppObject *)(s), (void**)&(s)->field, (value));\
    } while (0)

typedef struct Il2CppReflectionType {
    Il2CppObject object;
    const Il2CppType *type;
} Il2CppReflectionType;

// IMPORTANT: All managed types corresponding to the objects below must be blacklisted in mscorlib.xml

#if NET_4_0
// System.RuntimeType
typedef struct Il2CppReflectionRuntimeType
{
    Il2CppReflectionType type;
    Il2CppObject *type_info;
    Il2CppObject* genericCache;
    Il2CppObject* serializationCtor;
} Il2CppReflectionRuntimeType;
#endif

// System.MonoType
typedef struct Il2CppReflectionMonoType {
#if !NET_4_0
    Il2CppReflectionType type;
    Il2CppObject *type_info;
#else
    Il2CppReflectionRuntimeType type;
#endif

#ifdef __cplusplus

    const Il2CppType *GetIl2CppType() const {
#if !NET_4_0
        return type.type;
#else
        return type.type.type;
#endif
    }

#endif //__cplusplus
} Il2CppReflectionMonoType;

// System.Reflection.EventInfo
typedef struct Il2CppReflectionEvent {
    Il2CppObject object;
    Il2CppObject *cached_add_event;
} Il2CppReflectionEvent;

// System.Reflection.MonoEvent
typedef struct Il2CppReflectionMonoEvent {
    Il2CppReflectionEvent event;
    Il2CppReflectionType *reflectedType;
    const EventInfo *eventInfo;
} Il2CppReflectionMonoEvent;

// System.Reflection.MonoEventInfo
typedef struct Il2CppReflectionMonoEventInfo {
    Il2CppReflectionType *declaringType;
    Il2CppReflectionType *reflectedType;
    Il2CppString *name;
    Il2CppReflectionMethod *addMethod;
    Il2CppReflectionMethod *removeMethod;
    Il2CppReflectionMethod *raiseMethod;
    uint32_t eventAttributes;
    Il2CppArray *otherMethods;
} Il2CppReflectionMonoEventInfo;

#if !NET_4_0
// System.MonoEnumInfo
typedef struct Il2CppEnumInfo {
    Il2CppReflectionType *utype;
    Il2CppArray *values;
    Il2CppArray *names;
    void *name_hash;
} Il2CppEnumInfo;
#endif

// System.Reflection.MonoField
typedef struct Il2CppReflectionField {
    Il2CppObject object;
    Il2CppClass *klass;
    FieldInfo *field;
    Il2CppString *name;
    Il2CppReflectionType *type;
    uint32_t attrs;
} Il2CppReflectionField;

// System.Reflection.MonoProperty
typedef struct Il2CppReflectionProperty {
    Il2CppObject object;
    Il2CppClass *klass;
    const PropertyInfo *property;
} Il2CppReflectionProperty;

// System.Reflection.MonoMethod
typedef struct Il2CppReflectionMethod {
    Il2CppObject object;
    const MethodInfo *method;
    Il2CppString *name;
    Il2CppReflectionType *reftype;
} Il2CppReflectionMethod;

// System.Reflection.MonoGenericMethod
typedef struct Il2CppReflectionGenericMethod {
    Il2CppReflectionMethod base;
} Il2CppReflectionGenericMethod;

// System.Reflection.MonoMethodInfo
typedef struct Il2CppMethodInfo {
    Il2CppReflectionType *parent;
    Il2CppReflectionType *ret;
    uint32_t attrs;
    uint32_t implattrs;
    uint32_t callconv;
} Il2CppMethodInfo;

// System.Reflection.MonoPropertyInfo
typedef struct Il2CppPropertyInfo {
    Il2CppReflectionType *parent;
#if NET_4_0
    Il2CppReflectionType* declaringType;
#endif
    Il2CppString *name;
    Il2CppReflectionMethod *get;
    Il2CppReflectionMethod *set;
    uint32_t attrs;
} Il2CppPropertyInfo;

// System.Reflection.ParameterInfo
typedef struct Il2CppReflectionParameter {
    Il2CppObject object;
    Il2CppReflectionType *ClassImpl;
    Il2CppObject *DefaultValueImpl;
    Il2CppObject *MemberImpl;
    Il2CppString *NameImpl;
    int32_t PositionImpl;
    uint32_t AttrsImpl;
    Il2CppObject *MarshalAsImpl;
} Il2CppReflectionParameter;

// System.Reflection.Module
typedef struct Il2CppReflectionModule {
    Il2CppObject obj;
    const Il2CppImage *image;
    Il2CppReflectionAssembly *assembly;
    Il2CppString *fqname;
    Il2CppString *name;
    Il2CppString *scopename;
    bool is_resource;
    uint32_t token;
} Il2CppReflectionModule;

// System.Reflection.AssemblyName
typedef struct Il2CppReflectionAssemblyName {
    Il2CppObject obj;
    Il2CppString *name;
    Il2CppString *codebase;
    int32_t major, minor, build, revision;
    Il2CppObject *cultureInfo;
    uint32_t flags;
    uint32_t hashalg;
    Il2CppObject *keypair;
    Il2CppArray *publicKey;
    Il2CppArray *keyToken;
    uint32_t versioncompat;
    Il2CppObject *version;
    uint32_t processor_architecture;
#if NET_4_0
    uint32_t contentType;
#endif
} Il2CppReflectionAssemblyName;

// System.Reflection.Assembly
typedef struct Il2CppReflectionAssembly {
    Il2CppObject object;
    const Il2CppAssembly *assembly;
    Il2CppObject *resolve_event_holder;
    /* CAS related */
    Il2CppObject *evidence; /* Evidence */
    Il2CppObject *minimum;  /* PermissionSet - for SecurityAction.RequestMinimum */
    Il2CppObject *optional; /* PermissionSet - for SecurityAction.RequestOptional */
    Il2CppObject *refuse;   /* PermissionSet - for SecurityAction.RequestRefuse */
    Il2CppObject *granted;  /* PermissionSet - for the resolved assembly granted permissions */
    Il2CppObject *denied;   /* PermissionSet - for the resolved assembly denied permissions */
    /* */
    bool from_byte_array;
    Il2CppString *name;
} Il2CppReflectionAssembly;

// System.Reflection.Emit.UnmanagedMarshal
typedef struct Il2CppReflectionMarshal {
    Il2CppObject object;
    int32_t count;
    int32_t type;
    int32_t eltype;
    Il2CppString *guid;
    Il2CppString *mcookie;
    Il2CppString *marshaltype;
    Il2CppObject *marshaltyperef;
    int32_t param_num;
    bool has_size;
} Il2CppReflectionMarshal;

// System.Reflection.Pointer
typedef struct Il2CppReflectionPointer {
    Il2CppObject object;
    void *data;
    Il2CppReflectionType *type;
} Il2CppReflectionPointer;

#if NET_4_0
// System.Threading.InternalThread
typedef struct Il2CppInternalThread
{
    Il2CppObject obj;
    int lock_thread_id;
#ifdef __cplusplus
    il2cpp::os::Thread* handle;
#else
    void* handle;
#endif //__cplusplus
    void* native_handle;
    Il2CppArray* cached_culture_info;
    Il2CppChar* name;
    int name_len;
    uint32_t state;
    Il2CppObject* abort_exc;
    int abort_state_handle;
    uint64_t tid;
    intptr_t debugger_thread;
    void** static_data;
    void* runtime_thread_info;
    Il2CppObject* current_appcontext;
    Il2CppObject* root_domain_thread;
    Il2CppArray* _serialized_principal;
    int _serialized_principal_version;
    void* appdomain_refs;
    int32_t interruption_requested;
#ifdef __cplusplus
    il2cpp::os::FastMutex* synch_cs;
#else
    void* synch_cs;
#endif //__cplusplus
    bool threadpool_thread;
    bool thread_interrupt_requested;
    int stack_size;
    uint8_t apartment_state;
    int critical_region_level;
    int managed_id;
    uint32_t small_id;
    void* manage_callback;
    void* interrupt_on_stop;
    intptr_t flags;
    void* thread_pinning_ref;
    void* abort_protected_block_count;
    int32_t priority;
    void* owned_mutexes;
    void * suspended;
    int32_t self_suspended;
    size_t thread_state;
    size_t unused2;
    void* last;
} Il2CppInternalThread;

/* Keep in sync with System.IOSelectorJob in mcs/class/System/System/IOSelectorJob.cs */
typedef struct Il2CppIOSelectorJob
{
    Il2CppObject object;
    int32_t operation;
    Il2CppObject *callback;
    Il2CppObject *state;
} Il2CppIOSelectorJob;

/* This is a copy of System.Runtime.Remoting.Messaging.CallType */
typedef enum
{
    Il2Cpp_CallType_Sync = 0,
    Il2Cpp_CallType_BeginInvoke = 1,
    Il2Cpp_CallType_EndInvoke = 2,
    Il2Cpp_CallType_OneWay = 3
} Il2CppCallType;

typedef struct Il2CppMethodMessage
{
    Il2CppObject obj;
    Il2CppReflectionMethod *method;
    Il2CppArray  *args;
    Il2CppArray  *names;
    Il2CppArray  *arg_types;
    Il2CppObject *ctx;
    Il2CppObject *rval;
    Il2CppObject *exc;
    Il2CppAsyncResult *async_result;
    uint32_t        call_type;
} Il2CppMethodMessage;

#endif

/* This is a copy of System.AppDomainSetup */
typedef struct Il2CppAppDomainSetup {
    Il2CppObject object;
    Il2CppString *application_base;
    Il2CppString *application_name;
    Il2CppString *cache_path;
    Il2CppString *configuration_file;
    Il2CppString *dynamic_base;
    Il2CppString *license_file;
    Il2CppString *private_bin_path;
    Il2CppString *private_bin_path_probe;
    Il2CppString *shadow_copy_directories;
    Il2CppString *shadow_copy_files;
    uint8_t publisher_policy;
    uint8_t path_changed;
    int loader_optimization;
    uint8_t disallow_binding_redirects;
    uint8_t disallow_code_downloads;
    Il2CppObject *activation_arguments; /* it is System.Object in 1.x, ActivationArguments in 2.0 */
    Il2CppObject *domain_initializer;
    Il2CppObject *application_trust; /* it is System.Object in 1.x, ApplicationTrust in 2.0 */
    Il2CppArray *domain_initializer_args;
    uint8_t disallow_appbase_probe;
    Il2CppArray *configuration_bytes;
#if NET_4_0
    Il2CppArray* serialized_non_primitives;
#endif
} Il2CppAppDomainSetup;


// System.Threading.Thread
typedef struct Il2CppThread {
    Il2CppObject obj;
#if !NET_4_0
    int lock_thread_id; /* to be used as the pre-shifted thread id in thin locks */
    il2cpp::os::Thread *handle;
    Il2CppArray *cached_culture_info;
    void *unused1;
    bool threadpool_thread;//bool threadpool_thread;
    Il2CppChar *name;
    uint32_t name_len;
    uint32_t state;
    Il2CppObject *abort_exc;//MonoException *abort_exc;
    int abort_state_handle;
    uint64_t tid;   /* This is accessed as a gsize in the code (so it can hold a 64bit pointer on systems that need it), but needs to reserve 64 bits of space on all machines as it corresponds to a field in managed code */
    void *start_notify;
    void *stack_ptr;
    void **static_data;
    void *jit_data;
    void *lock_data;
    Il2CppObject *current_appcontext;//MonoAppContext *current_appcontext;
    int stack_size;
    Il2CppObject *start_obj;//MonoObject *start_obj;
    void *appdomain_refs;//GSList *appdomain_refs;
    ///* This is modified using atomic ops, so keep it a int32_t */
    int32_t interruption_requested;
    void *suspend_event;
    void *suspended_event;
    void *resume_event;
    il2cpp::os::FastMutex *synch_cs;
    uint8_t *serialized_culture_info;
    uint32_t serialized_culture_info_len;
    uint8_t *serialized_ui_culture_info;
    uint32_t serialized_ui_culture_info_len;
    bool thread_dump_requested;//bool thread_dump_requested;
    void *end_stack; /* This is only used when running in the debugger. */
    bool thread_interrupt_requested;//bool thread_interrupt_requested;
    uint8_t apartment_state;
    int32_t critical_region_level;
    uint32_t small_id; /* A small, unique id, used for the hazard pointer table. */
    void *manage_callback;
    Il2CppException *pending_exception;
    Il2CppObject *ec_to_set;
    ///*
    // * These fields are used to avoid having to increment corlib versions
    // * when a new field is added to the unmanaged MonoThread structure.
    // */
    void *interrupt_on_stop;
    uintptr_t flags;
    void *unused4;
    void *unused5;
    void *unused6;
    Il2CppObject *threadstart;
    int managed_id;
    Il2CppObject *principal;
    bool in_currentculture;
#else
    Il2CppInternalThread* internal_thread;
    Il2CppObject* start_obj;
    Il2CppException* pending_exception;
    Il2CppObject* principal;
    int32_t principal_version;
    Il2CppDelegate* delegate;
    Il2CppObject* executionContext;
    bool executionContextBelongsToOuterScope;
#endif

#ifdef __cplusplus
#if !NET_4_0

    Il2CppThread *GetInternalThread() {
        return this;
    }

#else
    Il2CppInternalThread* GetInternalThread() const
    {
        return internal_thread;
    }

#endif
#endif //__cplusplus
} Il2CppThread;

#ifdef __cplusplus
// System.Exception
typedef struct Il2CppException : public Il2CppObject {
#else
    typedef struct Il2CppException
    {
        Il2CppObject object;
#endif //__cplusplus
#if !NET_4_0
    /* Stores the IPs and the generic sharing infos
       (vtable/MRGCTX) of the frames. */
    Il2CppArray *trace_ips;
    Il2CppException *inner_ex;
    Il2CppString *message;
    Il2CppString *help_link;
    Il2CppString *class_name;
    Il2CppString *stack_trace;
    Il2CppString *remote_stack_trace;
    int32_t remote_stack_index;
    il2cpp_hresult_t hresult;
    Il2CppString *source;
    Il2CppObject *_data;
#else
    Il2CppString* className;
    Il2CppString* message;
    Il2CppObject* _data;
    Il2CppException* inner_ex;
    Il2CppString* _helpURL;
    Il2CppArray* trace_ips;
    Il2CppString* stack_trace;
    Il2CppString* remote_stack_trace;
    int remote_stack_index;
    Il2CppObject* _dynamicMethods;
    il2cpp_hresult_t hresult;
    Il2CppString* source;
    Il2CppObject* safeSerializationManager;
    Il2CppArray* captured_traces;
    Il2CppArray* native_trace_ips;
#endif
} Il2CppException;

// System.SystemException
typedef struct Il2CppSystemException {
    Il2CppException base;
} Il2CppSystemException;

// System.ArgumentException
typedef struct Il2CppArgumentException {
    Il2CppException base;
    Il2CppString *argName;
} Il2CppArgumentException;

// System.TypedReference
typedef struct Il2CppTypedRef {
    const Il2CppType *type;
    void *value;
    Il2CppClass *klass;
} Il2CppTypedRef;

// System.Delegate
typedef struct Il2CppDelegate {
    Il2CppObject object;
    /* The compiled code of the target method */
    Il2CppMethodPointer method_ptr;
    /* The invoke code */
    InvokerMethod invoke_impl;
    Il2CppObject *target;

#if RUNTIME_MONO
    const MonoMethod *method;
#else
    const MethodInfo *method;
#endif

    void *delegate_trampoline;

#if NET_4_0
    intptr_t extraArg;
#endif

    /*
     * If non-NULL, this points to a memory location which stores the address of
     * the compiled code of the method, or NULL if it is not yet compiled.
     */
    uint8_t **method_code;
    Il2CppReflectionMethod *method_info;
    Il2CppReflectionMethod *original_method_info;
    Il2CppObject *data;

#if NET_4_0
    bool method_is_virtual;
#endif
} Il2CppDelegate;

#if NET_4_0
typedef struct Il2CppMulticastDelegate
{
    Il2CppDelegate delegate;
    Il2CppArray *delegates;
} Il2CppMulticastDelegate;
#endif

// System.MarshalByRefObject
typedef struct Il2CppMarshalByRefObject {
    Il2CppObject obj;
    Il2CppObject *identity;
} Il2CppMarshalByRefObject;

#ifdef __cplusplus
struct QICache {
    const Il2CppGuid *iid;
    Il2CppIUnknown *qiResult;
};

// System.__Il2CppComObject (dummy type that replaces System.__ComObject)
struct Il2CppComObject : Il2CppObject {
    Il2CppIUnknown *identity;

    QICache qiShortCache[8];
    QICache *qiLongCache;
    int32_t qiShortCacheSize;
    int32_t qiLongCacheSize;
    int32_t qiLongCacheCapacity;

    // Same native object can be marshaled to managed code several times. If that happens,
    // we have to marshal it to the same RCW (same Il2CppComObject). We use a map of
    // IUnknown pointer -> weak GC handles to achieve it, and that works. When managed code
    // stops referencing the RCW, GC just garbage collects it and the finalizer will clean it
    // from our map. So far so good, eh?
    //
    // Enter Marshal.ReleaseComObject. This beast is designed to release the underlying COM object,
    // but ONLY after we used N amount of times (where N is the amount of times we marshaled
    // IUnknown into Il2CppComObject). In order to make it work, we need to implement ref counting.
    // This ref count gets incremented each time we marshal IUnknown to Il2CppComObject,
    // and gets decremented when Marshal.ReleaseComObject gets called. Fortunately, since we
    // live in a world of fairies and garbage collectors, we don't actually have to release it
    // manually in order for it to get cleaned up automatically in the future.
    volatile int32_t refCount;
};
#endif //__cplusplus

// System.AppDomain
typedef struct Il2CppAppDomain {
    Il2CppMarshalByRefObject mbr;
    Il2CppDomain *data;
} Il2CppAppDomain;

// System.Diagnostics.StackFrame
typedef struct Il2CppStackFrame {
    Il2CppObject obj;
    int32_t il_offset;
    int32_t native_offset;
#if NET_4_0
    uint64_t methodAddress;
    uint32_t methodIndex;
#endif
    Il2CppReflectionMethod *method;
    Il2CppString *filename;
    int32_t line;
    int32_t column;
    Il2CppString *internal_method_name;
} Il2CppStackFrame;

// System.Globalization.DateTimeFormatInfo
typedef struct Il2CppDateTimeFormatInfo {
    Il2CppObject obj;
#if !NET_4_0
    bool readOnly;
    Il2CppString *AMDesignator;
    Il2CppString *PMDesignator;
    Il2CppString *DateSeparator;
    Il2CppString *TimeSeparator;
    Il2CppString *ShortDatePattern;
    Il2CppString *LongDatePattern;
    Il2CppString *ShortTimePattern;
    Il2CppString *LongTimePattern;
    Il2CppString *MonthDayPattern;
    Il2CppString *YearMonthPattern;
    Il2CppString *FullDateTimePattern;
    Il2CppString *RFC1123Pattern;
    Il2CppString *SortableDateTimePattern;
    Il2CppString *UniversalSortableDateTimePattern;
    uint32_t FirstDayOfWeek;
    Il2CppObject *Calendar;
    uint32_t CalendarWeekRule;
    Il2CppArray *AbbreviatedDayNames;
    Il2CppArray *DayNames;
    Il2CppArray *MonthNames;
    Il2CppArray *AbbreviatedMonthNames;
    Il2CppArray *ShortDatePatterns;
    Il2CppArray *LongDatePatterns;
    Il2CppArray *ShortTimePatterns;
    Il2CppArray *LongTimePatterns;
    Il2CppArray *MonthDayPatterns;
    Il2CppArray *YearMonthPatterns;
    Il2CppArray *ShortDayNames;
#else
    Il2CppObject* CultureData;
    Il2CppString* Name;
    Il2CppString* LangName;
    Il2CppObject* CompareInfo;
    Il2CppObject* CultureInfo;
    Il2CppString* AMDesignator;
    Il2CppString* PMDesignator;
    Il2CppString* DateSeparator;
    Il2CppString* GeneralShortTimePattern;
    Il2CppString* GeneralLongTimePattern;
    Il2CppString* TimeSeparator;
    Il2CppString* MonthDayPattern;
    Il2CppString* DateTimeOffsetPattern;
    Il2CppObject* Calendar;
    uint32_t FirstDayOfWeek;
    uint32_t CalendarWeekRule;
    Il2CppString* FullDateTimePattern;
    Il2CppArray* AbbreviatedDayNames;
    Il2CppArray* ShortDayNames;
    Il2CppArray* DayNames;
    Il2CppArray* AbbreviatedMonthNames;
    Il2CppArray* MonthNames;
    Il2CppArray* GenitiveMonthNames;
    Il2CppArray* GenitiveAbbreviatedMonthNames;
    Il2CppArray* LeapYearMonthNames;
    Il2CppString* LongDatePattern;
    Il2CppString* ShortDatePattern;
    Il2CppString* YearMonthPattern;
    Il2CppString* LongTimePattern;
    Il2CppString* ShortTimePattern;
    Il2CppArray* YearMonthPatterns;
    Il2CppArray* ShortDatePatterns;
    Il2CppArray* LongDatePatterns;
    Il2CppArray* ShortTimePatterns;
    Il2CppArray* LongTimePatterns;
    Il2CppArray* EraNames;
    Il2CppArray* AbbrevEraNames;
    Il2CppArray* AbbrevEnglishEraNames;
    Il2CppArray* OptionalCalendars;
    bool readOnly;
    int32_t FormatFlags;
    int32_t CultureID;
    bool UseUserOverride;
    bool UseCalendarInfo;
    int32_t DataItem;
    bool IsDefaultCalendar;
    Il2CppArray* DateWords;
    Il2CppString* FullTimeSpanPositivePattern;
    Il2CppString* FullTimeSpanNegativePattern;
    Il2CppArray* dtfiTokenHash;
#endif
} Il2CppDateTimeFormatInfo;

// System.Globalization.NumberFormatInfo
typedef struct Il2CppNumberFormatInfo {
    Il2CppObject obj;
#if !NET_4_0
    bool readOnly;
    Il2CppString *decimalFormats;
    Il2CppString *currencyFormats;
    Il2CppString *percentFormats;
    Il2CppString *digitPattern;
    Il2CppString *zeroPattern;
    int32_t currencyDecimalDigits;
    Il2CppString *currencyDecimalSeparator;
    Il2CppString *currencyGroupSeparator;
    Il2CppArray *currencyGroupSizes;
    int32_t currencyNegativePattern;
    int32_t currencyPositivePattern;
    Il2CppString *currencySymbol;
    Il2CppString *naNSymbol;
    Il2CppString *negativeInfinitySymbol;
    Il2CppString *negativeSign;
    uint32_t numberDecimalDigits;
    Il2CppString *numberDecimalSeparator;
    Il2CppString *numberGroupSeparator;
    Il2CppArray *numberGroupSizes;
    int32_t numberNegativePattern;
    int32_t percentDecimalDigits;
    Il2CppString *percentDecimalSeparator;
    Il2CppString *percentGroupSeparator;
    Il2CppArray *percentGroupSizes;
    int32_t percentNegativePattern;
    int32_t percentPositivePattern;
    Il2CppString *percentSymbol;
    Il2CppString *perMilleSymbol;
    Il2CppString *positiveInfinitySymbol;
    Il2CppString *positiveSign;
#else
    Il2CppArray* numberGroupSizes;
    Il2CppArray* currencyGroupSizes;
    Il2CppArray* percentGroupSizes;
    Il2CppString* positiveSign;
    Il2CppString* negativeSign;
    Il2CppString* numberDecimalSeparator;
    Il2CppString* numberGroupSeparator;
    Il2CppString* currencyGroupSeparator;
    Il2CppString* currencyDecimalSeparator;
    Il2CppString* currencySymbol;
    Il2CppString* ansiCurrencySymbol;
    Il2CppString* naNSymbol;
    Il2CppString* positiveInfinitySymbol;
    Il2CppString* negativeInfinitySymbol;
    Il2CppString* percentDecimalSeparator;
    Il2CppString* percentGroupSeparator;
    Il2CppString* percentSymbol;
    Il2CppString* perMilleSymbol;
    Il2CppArray* nativeDigits;
    int dataItem;
    int numberDecimalDigits;
    int currencyDecimalDigits;
    int currencyPositivePattern;
    int currencyNegativePattern;
    int numberNegativePattern;
    int percentPositivePattern;
    int percentNegativePattern;
    int percentDecimalDigits;
    int digitSubstitution;
    bool readOnly;
    bool useUserOverride;
    bool isInvariant;
    bool validForParseAsNumber;
    bool validForParseAsCurrency;
#endif
} Il2CppNumberFormatInfo;

#if NET_4_0
typedef struct Il2CppCultureData
{
    Il2CppObject obj;
    Il2CppString *AMDesignator;
    Il2CppString *PMDesignator;
    Il2CppString *TimeSeparator;
    Il2CppArray *LongTimePatterns;
    Il2CppArray *ShortTimePatterns;
    uint32_t FirstDayOfWeek;
    uint32_t CalendarWeekRule;
} Il2CppCultureData;

typedef struct Il2CppCalendarData
{
    Il2CppObject obj;
    Il2CppString *NativeName;
    Il2CppArray *ShortDatePatterns;
    Il2CppArray *YearMonthPatterns;
    Il2CppArray *LongDatePatterns;
    Il2CppString *MonthDayPattern;

    Il2CppArray *EraNames;
    Il2CppArray *AbbreviatedEraNames;
    Il2CppArray *AbbreviatedEnglishEraNames;
    Il2CppArray *DayNames;
    Il2CppArray *AbbreviatedDayNames;
    Il2CppArray *SuperShortDayNames;
    Il2CppArray *MonthNames;
    Il2CppArray *AbbreviatedMonthNames;
    Il2CppArray *GenitiveMonthNames;
    Il2CppArray *GenitiveAbbreviatedMonthNames;
} Il2CppCalendarData;
#endif

// System.Globalization.CultureInfo
typedef struct Il2CppCultureInfo {
    Il2CppObject obj;
    bool is_read_only;
    int32_t lcid;
    int32_t parent_lcid;

#if !NET_4_0
    int32_t specific_lcid;
#endif

    int32_t datetime_index;
    int32_t number_index;

#if NET_4_0
    int32_t default_calendar_type;
#endif

    bool use_user_override;
    Il2CppNumberFormatInfo *number_format;
    Il2CppDateTimeFormatInfo *datetime_format;
    Il2CppObject *textinfo;
    Il2CppString *name;

#if !NET_4_0
    Il2CppString *displayname;
#endif

    Il2CppString *englishname;
    Il2CppString *nativename;
    Il2CppString *iso3lang;
    Il2CppString *iso2lang;

#if !NET_4_0
    Il2CppString *icu_name;
#endif

    Il2CppString *win3lang;
    Il2CppString *territory;

#if NET_4_0
    Il2CppArray* native_calendar_names;
#endif

    Il2CppString *compareinfo;

#if !NET_4_0
    const int32_t *calendar_data;
#endif

    const void *text_info_data;

#if NET_4_0
    int dataItem;
    Il2CppObject* calendar;
    Il2CppObject* parent_culture;
    bool constructed;
    Il2CppArray* cached_serialized_form;
    Il2CppObject* cultureData;
    bool isInherited;
#endif
} Il2CppCultureInfo;

// System.Globalization.RegionInfo
typedef struct Il2CppRegionInfo {
    Il2CppObject obj;
#if NET_4_0
    int32_t geo_id;
#else
    int32_t lcid;
    int32_t region_id;
#endif
    Il2CppString *iso2name;
    Il2CppString *iso3name;
    Il2CppString *win3name;
    Il2CppString *english_name;
#if NET_4_0
    Il2CppString* native_name;
#endif
    Il2CppString *currency_symbol;
    Il2CppString *iso_currency_symbol;
    Il2CppString *currency_english_name;
#if NET_4_0
    Il2CppString* currency_native_name;
#endif
} Il2CppRegionInfo;

// System.Runtime.InteropServices.SafeHandle
// Inherited by Microsoft.Win32.SafeHandles.SafeWaitHandle
typedef struct Il2CppSafeHandle {
    Il2CppObject base;
    void *handle;

#if !NET_4_0
    void *invalid_handle_value;
    int refcount;
    bool owns_handle;
#else
    int state;
    bool owns_handle;
    bool fullyInitialized;
#endif
} Il2CppSafeHandle;

// System.Text.StringBuilder
typedef struct Il2CppStringBuilder Il2CppStringBuilder;
typedef struct Il2CppStringBuilder {
    Il2CppObject object;

#if !NET_4_0
    int32_t length;
    Il2CppString *str;
    Il2CppString *cached_str;
    int32_t max_capacity;
#else
    Il2CppArray* chunkChars;
    Il2CppStringBuilder* chunkPrevious;
    int chunkLength;
    int chunkOffset;
    int maxCapacity;
#endif
} Il2CppStringBuilder;

// System.Net.SocketAddress
typedef struct Il2CppSocketAddress {
    Il2CppObject base;
#if !NET_4_0
    Il2CppArray *data;
#else
    int m_Size;
    Il2CppArray* data;
    bool m_changed;
    int m_hash;
#endif
} Il2CppSocketAddress;

// System.Globalization.SortKey
typedef struct Il2CppSortKey {
    Il2CppObject base;
    Il2CppString *str;
#if !NET_4_0
    int32_t options;
    Il2CppArray *key;
#else
    Il2CppArray *key;
    int32_t options;
#endif
    int32_t lcid;
} Il2CppSortKey;

// System.Runtime.InteropServices.ErrorWrapper
typedef struct Il2CppErrorWrapper {
    Il2CppObject base;
    int32_t errorCode;
} Il2CppErrorWrapper;

// System.Runtime.Remoting.Messaging.AsyncResult
typedef struct Il2CppAsyncResult {
    Il2CppObject base;
    Il2CppObject *async_state;
    Il2CppWaitHandle *handle;
    Il2CppDelegate *async_delegate;
    void *data; // We pass delegate arguments here. This is repurposed. Depends on Mono C# code not using the field.
    Il2CppAsyncCall *object_data;
    bool sync_completed;
    bool completed;
    bool endinvoke_called;
    Il2CppObject *async_callback;
    Il2CppObject *execution_context;
    Il2CppObject *original_context;
} Il2CppAsyncResult;

// System.MonoAsyncCall
typedef struct Il2CppAsyncCall {
    Il2CppObject base;

#if !NET_4_0
    void *msg; // We pass exceptions through here for now.
#else
    Il2CppMethodMessage *msg;
#endif

    MethodInfo *cb_method; // We don't set this.
    Il2CppDelegate *cb_target; // We pass the actual delegate here.
    Il2CppObject *state;
    Il2CppObject *res;
    Il2CppArray *out_args;
#if !NET_4_0
    /* This is a HANDLE, we use guint64 so the managed object layout remains constant */
    uint64_t wait_event;
#endif
} Il2CppAsyncCall;


#if RUNTIME_MONO
extern "C"
{
#include <mono/metadata/object.h>
}
#endif

typedef struct Il2CppExceptionWrapper Il2CppExceptionWrapper;
typedef struct Il2CppExceptionWrapper {
#if RUNTIME_MONO
    MonoException* ex;
#ifdef __cplusplus
    Il2CppExceptionWrapper(MonoException* ex) : ex(ex) {}
#endif //__cplusplus
#else
    Il2CppException *ex;
#ifdef __cplusplus

    Il2CppExceptionWrapper(Il2CppException *ex) : ex(ex) {}

#endif //__cplusplus
#endif
} Il2CppExceptionWrapper;

#if NET_4_0
typedef struct Il2CppIOAsyncResult
{
    Il2CppObject base;
    Il2CppDelegate* callback;
    Il2CppObject* state;
    Il2CppWaitHandle* wait_handle;
    bool completed_synchronously;
    bool completed;
} Il2CppIOAsyncResult;
#endif

/// Corresponds to Mono's internal System.Net.Sockets.Socket.SocketAsyncResult
/// class. Has no relation to Il2CppAsyncResult.
typedef struct Il2CppSocketAsyncResult {
#if !NET_4_0
    Il2CppObject base;
    Il2CppObject *socket;
    intptr_t handle;
    Il2CppObject *state;
    Il2CppDelegate *callback;
    Il2CppWaitHandle *wait_handle;
    Il2CppException *delayedException;
    Il2CppObject *ep;
    Il2CppArray *buffer;
    int32_t offset;
    int32_t size;
    int32_t socket_flags;
    Il2CppObject *accept_reuse_socket;
    Il2CppArray *addresses;
    int32_t port;
    Il2CppObject *buffers;
    bool reusesocket;
    Il2CppObject *acceptSocket;
    int32_t total;
    bool completed_synchronously;
    bool completed;
    bool blocking;
    int32_t error;
    int32_t operation;
    Il2CppAsyncResult *ares;
#else
    Il2CppIOAsyncResult base;
    Il2CppObject* socket;
    int32_t operation;
    Il2CppException* delayedException;
    Il2CppObject* endPoint;
    Il2CppArray* buffer;
    int32_t offset;
    int32_t size;
    int32_t socket_flags;
    Il2CppObject* acceptSocket;
    Il2CppArray* addresses;
    int32_t port;
    Il2CppObject* buffers;
    bool reuseSocket;
    int32_t currentAddress;
    Il2CppObject* acceptedSocket;
    int32_t total;
    int32_t error;
    int32_t endCalled;
#endif
} Il2CppSocketAsyncResult;

typedef enum Il2CppResourceLocation {
    IL2CPP_RESOURCE_LOCATION_EMBEDDED = 1,
    IL2CPP_RESOURCE_LOCATION_ANOTHER_ASSEMBLY = 2,
    IL2CPP_RESOURCE_LOCATION_IN_MANIFEST = 4
} Il2CppResourceLocation;

// System.Reflection.ManifestResourceInfo
typedef struct Il2CppManifestResourceInfo {
    Il2CppObject object;
    Il2CppReflectionAssembly *assembly;
    Il2CppString *filename;
    uint32_t location;
} Il2CppManifestResourceInfo;

#define IL2CPP_CHECK_ARG_NULL(arg)  do {    \
    if (arg == NULL)    \
    {   \
        il2cpp::vm::Exception::Raise (il2cpp::vm::Exception::GetArgumentNullException (#arg));  \
    };  } while (0)

// System.Runtime.Remoting.Contexts.Context
typedef struct Il2CppAppContext {
    Il2CppObject obj;
    int32_t domain_id;
    int32_t context_id;
    void *static_data;
} Il2CppAppContext;

#if !NET_4_0
// System.Decimal
typedef struct {
    //Note that we are not taking care of endianess.
    union {
        uint32_t ss32;
        struct signscale {
            unsigned int reserved1: 16;
            unsigned int scale: 8;
            unsigned int reserved2: 7;
            unsigned int sign: 1;
        } signscale;
    } u;
    uint32_t hi32;
    uint32_t lo32;
    uint32_t mid32;
} il2cpp_decimal_repr;
#else

typedef struct Il2CppDecimal
{
    // Decimal.cs treats the first two shorts as one long
    // And they seriable the data so we need to little endian
    // seriliazation
    // The wReserved overlaps with Variant's vt member
#if IL2CPP_BYTE_ORDER == IL2CPP_BIG_ENDIAN
    union
    {
        struct
        {
            uint8_t sign;
            uint8_t scale;
        } u;
        uint16_t signscale;
    } u;
    uint16_t reserved;
#else
    uint16_t reserved;
    union
    {
        struct
        {
            uint8_t scale;
            uint8_t sign;
        } u;
        uint16_t signscale;
    } u;
#endif
    uint32_t Hi32;
    union
    {
        struct
        {
            uint32_t Lo32;
            uint32_t Mid32;
        } v;
        uint64_t Lo64;
    } v;
} Il2CppDecimal;

// Structure to access an encoded double floating point
typedef struct Il2CppDouble
{
#if IL2CPP_BYTE_ORDER == IL2CPP_BIG_ENDIAN
    uint32_t sign : 1;
    uint32_t exp : 11;
    uint32_t mantHi : 20;
    uint32_t mantLo : 32;
#else // BIGENDIAN
    uint32_t mantLo : 32;
    uint32_t mantHi : 20;
    uint32_t exp : 11;
    uint32_t sign : 1;
#endif
} Il2CppDouble;

typedef union Il2CppDouble_double
{
    Il2CppDouble s;
    double d;
} Il2CppDouble_double;

typedef enum Il2CppDecimalCompareResult
{
    IL2CPP_DECIMAL_CMP_LT = -1,
    IL2CPP_DECIMAL_CMP_EQ,
    IL2CPP_DECIMAL_CMP_GT
} Il2CppDecimalCompareResult;

// Structure to access an encoded single floating point
typedef struct Il2CppSingle
{
#if IL2CPP_BYTE_ORDER == IL2CPP_BIG_ENDIAN
    uint32_t sign : 1;
    uint32_t exp : 8;
    uint32_t mant : 23;
#else
    uint32_t mant : 23;
    uint32_t exp : 8;
    uint32_t sign : 1;
#endif
} Il2CppSingle;

typedef union Il2CppSingle_float
{
    Il2CppSingle s;
    float f;
} Il2CppSingle_float;

#endif

#endif // IL2CPP_DOTS
