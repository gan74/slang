SLANG_RAW("// Slang `core` library\n")
SLANG_RAW("\n")
SLANG_RAW("// Modifier for variables that must resolve to compile-time constants\n")
SLANG_RAW("// as part of translation.\n")
SLANG_RAW("syntax constexpr : ConstExprModifier;\n")
SLANG_RAW("\n")
SLANG_RAW("// A type that can be used as an operand for builtins\n")
SLANG_RAW("interface __BuiltinType {}\n")
SLANG_RAW("\n")
SLANG_RAW("// A type that can be used for arithmetic operations\n")
SLANG_RAW("interface __BuiltinArithmeticType : __BuiltinType {}\n")
SLANG_RAW("\n")
SLANG_RAW("// A type that logically has a sign (positive/negative/zero)\n")
SLANG_RAW("interface __BuiltinSignedArithmeticType : __BuiltinArithmeticType {}\n")
SLANG_RAW("\n")
SLANG_RAW("// A type that can represent integers\n")
SLANG_RAW("interface __BuiltinIntegerType : __BuiltinArithmeticType\n")
SLANG_RAW("{}\n")
SLANG_RAW("\n")
SLANG_RAW("// A type that can represent non-integers\n")
SLANG_RAW("interface __BuiltinRealType : __BuiltinArithmeticType {}\n")
SLANG_RAW("\n")
SLANG_RAW("// A type that uses a floating-point representation\n")
SLANG_RAW("interface __BuiltinFloatingPointType : __BuiltinRealType, __BuiltinSignedArithmeticType\n")
SLANG_RAW("{\n")
SLANG_RAW("    // A builtin floating-point type must have an initializer that takes\n")
SLANG_RAW("    // a floating-point value...\n")
SLANG_RAW("    __init(float value);\n")
SLANG_RAW("}\n")
SLANG_RAW("\n")
SLANG_RAW("// A type resulting from an `enum` declaration.\n")
SLANG_RAW("__magic_type(EnumTypeType)\n")
SLANG_RAW("interface __EnumType\n")
SLANG_RAW("{\n")
SLANG_RAW("    // The type of tags for this `enum`\n")
SLANG_RAW("    //\n")
SLANG_RAW("    // Note: using `__Tag` instead of `Tag` to avoid any\n")
SLANG_RAW("    // conflict if a user had an `enum` case called `Tag`\n")
SLANG_RAW("    associatedtype __Tag : __BuiltinIntegerType;\n")
SLANG_RAW("};\n")
SLANG_RAW("\n")
SLANG_RAW("// A type resulting from an `enum` declaration\n")
SLANG_RAW("// with the `[flags]` attribute.\n")
SLANG_RAW("interface __FlagsEnumType : __EnumType\n")
SLANG_RAW("{\n")
SLANG_RAW("};\n")
SLANG_RAW("\n")
SLANG_RAW("__generic<T,U> __intrinsic_op(Sequence) U operator,(T left, U right);\n")
SLANG_RAW("\n")
SLANG_RAW("__generic<T> __intrinsic_op(select) T operator?:(bool condition, T ifTrue, T ifFalse);\n")
SLANG_RAW("__generic<T, let N : int> __intrinsic_op(select) vector<T,N> operator?:(vector<bool,N> condition, vector<T,N> ifTrue, vector<T,N> ifFalse);\n")
SLANG_RAW("\n")

// We are going to use code generation to produce the
// declarations for all of our base types.

static const int kBaseTypeCount = sizeof(kBaseTypes) / sizeof(kBaseTypes[0]);
for (int tt = 0; tt < kBaseTypeCount; ++tt)
{
    EMIT_LINE_DIRECTIVE();
    sb << "__builtin_type(" << int(kBaseTypes[tt].tag) << ") struct " << kBaseTypes[tt].name;

    // Declare interface conformances for this type

    sb << "\n    : __BuiltinType\n";

    switch (kBaseTypes[tt].tag)
    {
    case BaseType::Float:
        sb << "\n    , __BuiltinFloatingPointType\n";
        sb << "\n    ,  __BuiltinRealType\n";
        // fall through to:
    case BaseType::Int:
        sb << "\n    ,  __BuiltinSignedArithmeticType\n";
        // fall through to:
    case BaseType::UInt:
    case BaseType::UInt64:
        sb << "\n    ,  __BuiltinArithmeticType\n";
        // fall through to:
    case BaseType::Bool:
        sb << "\n    ,  __BuiltinType\n";
        break;

    default:
        break;
    }

    sb << "\n{\n";


    // Declare initializers to convert from various other types
    for (int ss = 0; ss < kBaseTypeCount; ++ss)
    {
        // Don't allow conversion from `void`
        if (kBaseTypes[ss].tag == BaseType::Void)
            continue;

        // We need to emit a modifier so that the semantic-checking
        // layer will know it can use these operations for implicit
        // conversion.
        ConversionCost conversionCost = getBaseTypeConversionCost(
            kBaseTypes[tt],
            kBaseTypes[ss]);

        EMIT_LINE_DIRECTIVE();
        sb << "__implicit_conversion(" << conversionCost << ")\n";

        EMIT_LINE_DIRECTIVE();
        sb << "__init(" << kBaseTypes[ss].name << " value);\n";
    }

    // If this is a basic integer type, then define explicit
    // initializers that take a value of an `enum` type.
    //
    // TODO: This should actually be restricted, so that this
    // only applies `where T.__Tag == Self`, but we don't have
    // the needed features in our type system to implement
    // that constraint right now.
    //
    switch (kBaseTypes[tt].tag)
    {
    case BaseType::Int:
    case BaseType::UInt:
SLANG_RAW("\n")
SLANG_RAW("        __generic<T:__EnumType>\n")
SLANG_RAW("        __init(T value);\n")

        break;

    default:
        break;
    }

    sb << "};\n";
}

// Declare built-in pointer type
// (eventually we can have the traditional syntax sugar for this)
SLANG_RAW("\n")
SLANG_RAW("\n")
SLANG_RAW("__generic<T>\n")
SLANG_RAW("__magic_type(PtrType)\n")
SLANG_RAW("__intrinsic_type(")
SLANG_SPLICE(kIROp_PtrType
)
SLANG_RAW(")\n")
SLANG_RAW("struct Ptr\n")
SLANG_RAW("{};\n")
SLANG_RAW("\n")
SLANG_RAW("__generic<T>\n")
SLANG_RAW("__magic_type(OutType)\n")
SLANG_RAW("__intrinsic_type(")
SLANG_SPLICE(kIROp_OutType
)
SLANG_RAW(")\n")
SLANG_RAW("struct Out\n")
SLANG_RAW("{};\n")
SLANG_RAW("\n")
SLANG_RAW("__generic<T>\n")
SLANG_RAW("__magic_type(InOutType)\n")
SLANG_RAW("__intrinsic_type(")
SLANG_SPLICE(kIROp_InOutType
)
SLANG_RAW(")\n")
SLANG_RAW("struct InOut\n")
SLANG_RAW("{};\n")
SLANG_RAW("\n")
SLANG_RAW("__generic<T>\n")
SLANG_RAW("__magic_type(RefType)\n")
SLANG_RAW("__intrinsic_type(")
SLANG_SPLICE(kIROp_RefType
)
SLANG_RAW(")\n")
SLANG_RAW("struct Ref\n")
SLANG_RAW("{};\n")
SLANG_RAW("\n")
SLANG_RAW("__magic_type(StringType)\n")
SLANG_RAW("__intrinsic_type(")
SLANG_SPLICE(kIROp_StringType
)
SLANG_RAW(")\n")
SLANG_RAW("struct String\n")
SLANG_RAW("{};\n")
SLANG_RAW("\n")

// Declare vector and matrix types

sb << "__generic<T = float, let N : int = 4> __magic_type(Vector) struct vector\n{\n";
sb << "    typedef T Element;\n";

// Declare initializer taking a single scalar of the elemnt type
sb << "    __implicit_conversion(" << kConversionCost_ScalarToVector << ")\n";
sb << "    __intrinsic_op(" << kIROp_constructVectorFromScalar << ")\n";
sb << "    __init(T value);\n";

// Allow initialization from same type
sb << "    __init(vector<T,N> value);\n";

sb << "};\n";
SLANG_RAW("\n")
SLANG_RAW("\n")
SLANG_RAW("__generic<T = float, let R : int = 4, let C : int = 4>\n")
SLANG_RAW("__magic_type(Matrix)\n")
SLANG_RAW("struct matrix {};\n")
SLANG_RAW("\n")

static const struct {
    char const* name;
    char const* glslPrefix;
} kTypes[] =
{
    {"float", ""},
    {"int", "i"},
    {"uint", "u"},
    {"bool", "b"},
};
static const int kTypeCount = sizeof(kTypes) / sizeof(kTypes[0]);

for (int tt = 0; tt < kTypeCount; ++tt)
{
    // Declare HLSL vector types
    for (int ii = 1; ii <= 4; ++ii)
    {
        sb << "typedef vector<" << kTypes[tt].name << "," << ii << "> " << kTypes[tt].name << ii << ";\n";
    }

    // Declare HLSL matrix types
    for (int rr = 2; rr <= 4; ++rr)
    for (int cc = 2; cc <= 4; ++cc)
    {
        sb << "typedef matrix<" << kTypes[tt].name << "," << rr << "," << cc << "> " << kTypes[tt].name << rr << "x" << cc << ";\n";
    }
}

// Declare additional built-in generic types
//        EMIT_LINE_DIRECTIVE();


sb << "__generic<T>\n";
sb << "__intrinsic_type(" << kIROp_ConstantBufferType << ")\n";
sb << "__magic_type(ConstantBuffer) struct ConstantBuffer {};\n";

sb << "__generic<T>\n";
sb << "__intrinsic_type(" << kIROp_TextureBufferType << ")\n";
sb << "__magic_type(TextureBuffer) struct TextureBuffer {};\n";

sb << "__generic<T>\n";
sb << "__intrinsic_type(" << kIROp_ParameterBlockType << ")\n";
sb << "__magic_type(ParameterBlockType) struct ParameterBlock {};\n";

static const char* kComponentNames[]{ "x", "y", "z", "w" };
static const char* kVectorNames[]{ "", "x", "xy", "xyz", "xyzw" };

// Need to add constructors to the types above
for (int N = 2; N <= 4; ++N)
{
    sb << "__generic<T> __extension vector<T, " << N << ">\n{\n";

    // initialize from N scalars
    sb << "__init(";
    for (int ii = 0; ii < N; ++ii)
    {
        if (ii != 0) sb << ", ";
        sb << "T " << kComponentNames[ii];
    }
    sb << ");\n";

    // Initialize from an M-vector and then scalars
    for (int M = 2; M < N; ++M)
    {
        sb << "__init(vector<T," << M << "> " << kVectorNames[M];
        for (int ii = M; ii < N; ++ii)
        {
            sb << ", T " << kComponentNames[ii];
        }
        sb << ");\n";
    }

    // Initialize from two vectors, of size M and N-M
    for(int M = 2; M <= (N-2); ++M)
    {
        int K = N - M;
        SLANG_ASSERT(K >= 2);

        sb << "__init(vector<T," << M << "> " << kVectorNames[M];
        sb << ", vector<T," << K << "> ";
        for (int ii = 0; ii < K; ++ii)
        {
            sb << kComponentNames[ii];
        }
        sb << ");\n";
    }

    sb << "}\n";
}

// The above extension was generic in the *type* of the vector,
// but explicit in the *size*. We will now declare an extension
// for each builtin type that is generic in the size.
//
for (int tt = 0; tt < kBaseTypeCount; ++tt)
{
    if(kBaseTypes[tt].tag == BaseType::Void) continue;

    sb << "__generic<let N : int> __extension vector<"
        << kBaseTypes[tt].name << ",N>\n{\n";

    for (int ff = 0; ff < kBaseTypeCount; ++ff)
    {
        if(kBaseTypes[ff].tag == BaseType::Void) continue;


        if( tt != ff )
        {
            auto cost = getBaseTypeConversionCost(
                kBaseTypes[tt],
                kBaseTypes[ff]);

			// Implicit conversion from a vector of the same
			// size, but different element type.
            sb << "    __implicit_conversion(" << cost << ")\n";
            sb << "    __init(vector<" << kBaseTypes[ff].name << ",N> value);\n";

			// Constructor to make a vector from a scalar of another type.
            cost += kConversionCost_ScalarToVector;
            sb << "    __implicit_conversion(" << cost << ")\n";
            sb << "    __init(" << kBaseTypes[ff].name << " value);\n";
        }
    }

    sb << "}\n";
}

for( int R = 2; R <= 4; ++R )
for( int C = 2; C <= 4; ++C )
{
    sb << "__generic<T> __extension matrix<T, " << R << "," << C << ">\n{\n";

    // initialize from R*C scalars
    sb << "__init(";
    for( int ii = 0; ii < R; ++ii )
    for( int jj = 0; jj < C; ++jj )
    {
        if ((ii+jj) != 0) sb << ", ";
        sb << "T m" << ii << jj;
    }
    sb << ");\n";

    // Initialize from R C-vectors
    sb << "__init(";
    for (int ii = 0; ii < R; ++ii)
    {
        if(ii != 0) sb << ", ";
        sb << "vector<T," << C << "> row" << ii;
    }
    sb << ");\n";


    // initialize from another matrix of the same size
    //
    // TODO(tfoley): See comment about how this overlaps
    // with implicit conversion, in the `vector` case above
    sb << "__generic<U> __init(matrix<U," << R << ", " << C << ">);\n";

    // initialize from a matrix of larger size
    for(int rr = R; rr <= 4; ++rr)
    for( int cc = C; cc <= 4; ++cc )
    {
        if(rr == R && cc == C) continue;
        sb << "__init(matrix<T," << rr << "," << cc << "> value);\n";
    }

    sb << "}\n";
}

// Declare built-in texture and sampler types



sb << "__magic_type(SamplerState," << int(SamplerStateFlavor::SamplerState) << ")\n";
sb << "__intrinsic_type(" << kIROp_SamplerStateType << ")\n";
sb << "struct SamplerState {};";

sb << "__magic_type(SamplerState," << int(SamplerStateFlavor::SamplerComparisonState) << ")\n";
sb << "__intrinsic_type(" << kIROp_SamplerComparisonStateType << ")\n";
sb << "struct SamplerComparisonState {};";

// TODO(tfoley): Need to handle `RW*` variants of texture types as well...
static const struct {
    char const*			    name;
    TextureFlavor::Shape	baseShape;
    int					    coordCount;
} kBaseTextureTypes[] = {
    { "Texture1D",		TextureFlavor::Shape::Shape1D,	1 },
    { "Texture2D",		TextureFlavor::Shape::Shape2D,	2 },
    { "Texture3D",		TextureFlavor::Shape::Shape3D,	3 },
    { "TextureCube",	TextureFlavor::Shape::ShapeCube,	3 },
};
static const int kBaseTextureTypeCount = sizeof(kBaseTextureTypes) / sizeof(kBaseTextureTypes[0]);


static const struct {
    char const*         name;
    SlangResourceAccess access;
} kBaseTextureAccessLevels[] = {
    { "",                   SLANG_RESOURCE_ACCESS_READ },
    { "RW",                 SLANG_RESOURCE_ACCESS_READ_WRITE },
    { "RasterizerOrdered",  SLANG_RESOURCE_ACCESS_RASTER_ORDERED },
};
static const int kBaseTextureAccessLevelCount = sizeof(kBaseTextureAccessLevels) / sizeof(kBaseTextureAccessLevels[0]);

// Declare the GLSL types here for compatibility...
//
// TODO: The stdlib should include a module that declares the GLSL types, to keep
// them separate...
for (int tt = 0; tt < kBaseTextureTypeCount; ++tt)
{
    char const* name = kBaseTextureTypes[tt].name;
    TextureFlavor::Shape baseShape = kBaseTextureTypes[tt].baseShape;

    for (int isArray = 0; isArray < 2; ++isArray)
    {
        // Arrays of 3D textures aren't allowed
        if (isArray && baseShape == TextureFlavor::Shape::Shape3D) continue;

        for (int isMultisample = 0; isMultisample < 2; ++isMultisample)
        for (int accessLevel = 0; accessLevel < kBaseTextureAccessLevelCount; ++accessLevel)
        {
            auto access = kBaseTextureAccessLevels[accessLevel].access;

            // TODO: any constraints to enforce on what gets to be multisampled?

            unsigned flavor = baseShape;
            if (isArray)		flavor |= TextureFlavor::ArrayFlag;
            if (isMultisample)	flavor |= TextureFlavor::MultisampleFlag;
//                        if (isShadow)		flavor |= TextureFlavor::ShadowFlag;

            flavor |= (access << 8);

            // emit a generic signature
            // TODO: allow for multisample count to come in as well...
            sb << "__generic<T = float4> ";

            sb << "__magic_type(TextureSampler," << int(flavor) << ")\n";
            sb << "__intrinsic_type(" << (kIROp_FirstTextureSamplerType + flavor) << ")\n";
            sb << "struct Sampler";
            sb << kBaseTextureAccessLevels[accessLevel].name;
            sb << name;
            if (isMultisample) sb << "MS";
            if (isArray) sb << "Array";
//                        if (isShadow) sb << "Shadow";
            sb << "\n{\n";
            sb << "__specialized_for_target(glsl)\n";
			sb << "__init(";
			sb << kBaseTextureAccessLevels[accessLevel].name;
            sb << name;
            if (isMultisample) sb << "MS";
            if (isArray) sb << "Array";
			sb << "<T> t, ";
			sb << "SamplerState s);\n";
			sb << "};\n";

            sb << "__specialized_for_target(glsl)\n";
            sb << "T texture<T>(Sampler";
			sb << kBaseTextureAccessLevels[accessLevel].name;
            sb << name;
            if (isMultisample) sb << "MS";
            if (isArray) sb << "Array";
			sb << "<T> t, float" << kBaseTextureTypes[tt].coordCount + isArray << " location);\n";
		}
	}
}

for (int tt = 0; tt < kBaseTextureTypeCount; ++tt)
{
    char const* name = kBaseTextureTypes[tt].name;
    TextureFlavor::Shape baseShape = kBaseTextureTypes[tt].baseShape;

    for (int isArray = 0; isArray < 2; ++isArray)
    {
        // Arrays of 3D textures aren't allowed
        if (isArray && baseShape == TextureFlavor::Shape::Shape3D) continue;

        for (int isMultisample = 0; isMultisample < 2; ++isMultisample)
        for (int accessLevel = 0; accessLevel < kBaseTextureAccessLevelCount; ++accessLevel)
        {
            auto access = kBaseTextureAccessLevels[accessLevel].access;

            // TODO: any constraints to enforce on what gets to be multisampled?

            unsigned flavor = baseShape;
            if (isArray)		flavor |= TextureFlavor::ArrayFlag;
            if (isMultisample)	flavor |= TextureFlavor::MultisampleFlag;
//                        if (isShadow)		flavor |= TextureFlavor::ShadowFlag;

            flavor |= (access << 8);

            // emit a generic signature
            // TODO: allow for multisample count to come in as well...
            sb << "__generic<T = float4> ";

            sb << "__magic_type(Texture," << int(flavor) << ")\n";
            sb << "__intrinsic_type(" << (kIROp_FirstTextureType + flavor) << ")\n";
            sb << "struct ";
            sb << kBaseTextureAccessLevels[accessLevel].name;
            sb << name;
            if (isMultisample) sb << "MS";
            if (isArray) sb << "Array";
//                        if (isShadow) sb << "Shadow";
            sb << "\n{";

            if( !isMultisample )
            {
                sb << "float CalculateLevelOfDetail(SamplerState s, ";
                sb << "float" << kBaseTextureTypes[tt].coordCount << " location);\n";

                sb << "float CalculateLevelOfDetailUnclamped(SamplerState s, ";
                sb << "float" << kBaseTextureTypes[tt].coordCount << " location);\n";
            }

            // `GetDimensions`

            for(int isFloat = 0; isFloat < 2; ++isFloat)
            for(int includeMipInfo = 0; includeMipInfo < 2; ++includeMipInfo)
            {
                {
                    sb << "__glsl_version(450)\n";
                    sb << "__target_intrinsic(glsl, \"(";

                    int aa = 1;
                    String lodStr = ", 0";
                    if (includeMipInfo)
                    {
                        int mipLevelArg = aa++;
                        lodStr = ", int($";
                        lodStr.append(mipLevelArg);
                        lodStr.append(")");
                    }

                    String opStr = " = textureSize($P" + lodStr;
                    switch( access )
                    {
                    case SLANG_RESOURCE_ACCESS_READ_WRITE:
                    case SLANG_RESOURCE_ACCESS_RASTER_ORDERED:
                        opStr = " = imageSize($0";
                        break;

                    default:
                        break;
                    }


                    int cc = 0;
                    switch(baseShape)
                    {
                    case TextureFlavor::Shape::Shape1D:
                        sb << "($" << aa++ << opStr << "))";
                        cc = 1;
                        break;

                    case TextureFlavor::Shape::Shape2D:
                    case TextureFlavor::Shape::ShapeCube:
                        sb << "($" << aa++ << opStr << ").x)";
                        sb << ", ($" << aa++ << opStr << ").y)";
                        cc = 2;
                        break;

                    case TextureFlavor::Shape::Shape3D:
                        sb << "($" << aa++ << opStr << ").x)";
                        sb << ", ($" << aa++ << opStr << ").y)";
                        sb << ", ($" << aa++ << opStr << ").z)";
                        cc = 3;
                        break;

                    default:
                        SLANG_UNEXPECTED("unhandled resource shape");
                        break;
                    }

                    if(isArray)
                    {
                        sb << ", ($" << aa++ << opStr << ")." << kComponentNames[cc] << ")";
                    }

                    if(isMultisample)
                    {
                        sb << ", ($" << aa++ << " = textureSamples($P))";
                    }

                    if (includeMipInfo)
                    {
                        sb << ", ($" << aa++ << " = textureQueryLevels($P))";
                    }


                    sb << ")\")\n";
                }

                char const* t = isFloat ? "out float " : "out uint ";

                sb << "void GetDimensions(";
                if(includeMipInfo)
                    sb << "uint mipLevel, ";

                switch(baseShape)
                {
                case TextureFlavor::Shape::Shape1D:
                    sb << t << "width";
                    break;

                case TextureFlavor::Shape::Shape2D:
                case TextureFlavor::Shape::ShapeCube:
                    sb << t << "width,";
                    sb << t << "height";
                    break;

                case TextureFlavor::Shape::Shape3D:
                    sb << t << "width,";
                    sb << t << "height,";
                    sb << t << "depth";
                    break;

                default:
                    assert(!"unexpected");
                    break;
                }

                if(isArray)
                {
                    sb << ", " << t << "elements";
                }

                if(isMultisample)
                {
                    sb << ", " << t << "sampleCount";
                }

                if(includeMipInfo)
                    sb << ", " << t << "numberOfLevels";

                sb << ");\n";
            }

            // `GetSamplePosition()`
            if( isMultisample )
            {
                sb << "float2 GetSamplePosition(int s);\n";
            }

            // `Load()`

            if( kBaseTextureTypes[tt].coordCount + isArray < 4 )
            {
                int loadCoordCount = kBaseTextureTypes[tt].coordCount + isArray + (isMultisample?0:1);

                // When translating to GLSL, we need to break apart the `location` argument.
                //
                // TODO: this should realy be handled by having this member actually get lowered!
                static const char* kGLSLLoadCoordsSwizzle[] = { "", "", "x", "xy", "xyz", "xyzw" };
                static const char* kGLSLLoadLODSwizzle[]    = { "", "", "y", "z", "w", "error" };

                if (isMultisample)
                {
                    sb << "__target_intrinsic(glsl, \"texelFetch($P, $1, $3)\")\n";
                }
                else
                {
                    sb << "__target_intrinsic(glsl, \"texelFetch($P, ($1)." << kGLSLLoadCoordsSwizzle[loadCoordCount] << ", ($1)." << kGLSLLoadLODSwizzle[loadCoordCount] << ")\")\n";
                }
                sb << "T Load(";
                sb << "int" << loadCoordCount << " location";
                if(isMultisample)
                {
                    sb << ", int sampleIndex";
                }
                sb << ");\n";

                if (isMultisample)
                {
                    sb << "__target_intrinsic(glsl, \"texelFetchOffset($P, $0, $1, $2)\")\n";
                }
                else
                {
                    sb << "__target_intrinsic(glsl, \"texelFetch($P, ($1)." << kGLSLLoadCoordsSwizzle[loadCoordCount] << ", ($1)." << kGLSLLoadLODSwizzle[loadCoordCount] << ", $2)\")\n";
                }
                sb << "T Load(";
                sb << "int" << loadCoordCount << " location";
                if(isMultisample)
                {
                    sb << ", int sampleIndex";
                }
                sb << ", constexpr int" << loadCoordCount << " offset";
                sb << ");\n";


                sb << "T Load(";
                sb << "int" << loadCoordCount << " location";
                if(isMultisample)
                {
                    sb << ", int sampleIndex";
                }
                sb << ", constexpr int" << kBaseTextureTypes[tt].coordCount << " offset";
                sb << ", out uint status";
                sb << ");\n";
            }

            if(baseShape != TextureFlavor::Shape::ShapeCube)
            {
                int N = kBaseTextureTypes[tt].coordCount + isArray;

                char const* uintNs[] = { "", "uint", "uint2", "uint3", "uint4" };
                char const* ivecNs[] = {  "", "int", "ivec2", "ivec3", "ivec4" };

                auto uintN = uintNs[N];
                auto ivecN = ivecNs[N];

                // subscript operator
                sb << "__subscript(" << uintN << " location) -> T {\n";

                sb << "__target_intrinsic(glsl, \"texelFetch($P, " << ivecN << "($1)";

                if( !isMultisample )
                {
                    sb << ", 0";
                }
                else
                {
                    // TODO: how to handle this...
                    sb << ", 0";
                }

                sb << ")$z\") get;\n";

                // Depending on the access level of the texture type,
                // we either have just a getter (the default), or both
                // a getter and setter.
                switch( access )
                {
                case SLANG_RESOURCE_ACCESS_NONE:
                case SLANG_RESOURCE_ACCESS_READ:
                    break;

                default:
                    sb << "__target_intrinsic(glsl, \"imageStore($0, " << ivecN << "($1), $2)\") set;\n";

                    // TODO: really need a way to map access through `ref` accessor (e.g., when
                    // used with an atomic operation) over to GLSL equivalent.
                    //
                    sb << "ref;\n";
                    break;
                }

                sb << "}\n";
            }

            if( !isMultisample )
            {
                // `Sample()`

                sb << "__target_intrinsic(glsl, \"texture($p, $2)\")\n";

                // TODO: only enable if IR is being used?
//                sb << "__intrinsic_op(sample)\n";

                sb << "T Sample(SamplerState s, ";
                sb << "float" << kBaseTextureTypes[tt].coordCount + isArray << " location);\n";

                if( baseShape != TextureFlavor::Shape::ShapeCube )
                {
                    sb << "__target_intrinsic(glsl, \"textureOffset($p, $2, $3)\")\n";
                    sb << "T Sample(SamplerState s, ";
                    sb << "float" << kBaseTextureTypes[tt].coordCount + isArray << " location, ";
                    sb << "constexpr int" << kBaseTextureTypes[tt].coordCount << " offset);\n";
                }

                sb << "T Sample(SamplerState s, ";
                sb << "float" << kBaseTextureTypes[tt].coordCount + isArray << " location, ";
                if( baseShape != TextureFlavor::Shape::ShapeCube )
                {
                    sb << "constexpr int" << kBaseTextureTypes[tt].coordCount << " offset, ";
                }
                sb << "float clamp);\n";

                sb << "T Sample(SamplerState s, ";
                sb << "float" << kBaseTextureTypes[tt].coordCount + isArray << " location, ";
                if( baseShape != TextureFlavor::Shape::ShapeCube )
                {
                    sb << "constexpr int" << kBaseTextureTypes[tt].coordCount << " offset, ";
                }
                sb << "float clamp, out uint status);\n";


                // `SampleBias()`
                sb << "__target_intrinsic(glsl, \"texture($p, $2, $3)\")\n";
                sb << "T SampleBias(SamplerState s, ";
                sb << "float" << kBaseTextureTypes[tt].coordCount + isArray << " location, float bias);\n";

                if( baseShape != TextureFlavor::Shape::ShapeCube )
                {
                    sb << "__target_intrinsic(glsl, \"textureOffset($p, $2, $3, $4)\")\n";
                    sb << "T SampleBias(SamplerState s, ";
                    sb << "float" << kBaseTextureTypes[tt].coordCount + isArray << " location, float bias, ";
                    sb << "constexpr int" << kBaseTextureTypes[tt].coordCount << " offset);\n";
                }

                // `SampleCmp()` and `SampleCmpLevelZero`
                sb << "float SampleCmp(SamplerComparisonState s, ";
                sb << "float" << kBaseTextureTypes[tt].coordCount + isArray << " location, ";
                sb << "float compareValue";
                sb << ");\n";

                int baseCoordCount = kBaseTextureTypes[tt].coordCount;
                int arrCoordCount = baseCoordCount + isArray;
                if (arrCoordCount < 3)
                {
                    int extCoordCount = arrCoordCount + 1;

                    if (extCoordCount < 3)
                        extCoordCount = 3;

                    sb << "__target_intrinsic(glsl, \"textureLod($p, ";

                    sb << "vec" << extCoordCount << "($2,";
                    for (int ii = arrCoordCount; ii < extCoordCount - 1; ++ii)
                    {
                        sb << " 0.0,";
                    }
                    sb << "$3)";

                    sb << ", 0.0)\")\n";
                }
                else if(arrCoordCount <= 3)
                {
                    int extCoordCount = arrCoordCount + 1;

                    if (extCoordCount < 3)
                        extCoordCount = 3;

                    sb << "__target_intrinsic(glsl, \"textureGrad($p, ";

                    sb << "vec" << extCoordCount << "($2,";
                    for (int ii = arrCoordCount; ii < extCoordCount - 1; ++ii)
                    {
                        sb << " 0.0,";
                    }
                    sb << "$3)";

                    // Construct gradients
                    sb << ", vec" << baseCoordCount << "(0.0)";
                    sb << ", vec" << baseCoordCount << "(0.0)";
                    sb << ")\")\n";
                }
                sb << "float SampleCmpLevelZero(SamplerComparisonState s, ";
                sb << "float" << kBaseTextureTypes[tt].coordCount + isArray << " location, ";
                sb << "float compareValue";
                sb << ");\n";

                if( baseShape != TextureFlavor::Shape::ShapeCube )
                {
                    // Note(tfoley): MSDN seems confused, and claims that the `offset`
                    // parameter for `SampleCmp` is available for everything but 3D
                    // textures, while `Sample` and `SampleBias` are consistent in
                    // saying they only exclude `offset` for cube maps (which makes
                    // sense). I'm going to assume the documentation for `SampleCmp`
                    // is just wrong.

                    sb << "float SampleCmp(SamplerComparisonState s, ";
                    sb << "float" << kBaseTextureTypes[tt].coordCount + isArray << " location, ";
                    sb << "float compareValue, ";
                    sb << "constexpr int" << kBaseTextureTypes[tt].coordCount << " offset);\n";

                    sb << "float SampleCmpLevelZero(SamplerComparisonState s, ";
                    sb << "float" << kBaseTextureTypes[tt].coordCount + isArray << " location, ";
                    sb << "float compareValue, ";
                    sb << "constexpr int" << kBaseTextureTypes[tt].coordCount << " offset);\n";
                }


                sb << "__target_intrinsic(glsl, \"textureGrad($p, $2, $3, $4)\")\n";
//                sb << "__intrinsic_op(sampleGrad)\n";
                sb << "T SampleGrad(SamplerState s, ";
                sb << "float" << kBaseTextureTypes[tt].coordCount + isArray << " location, ";
                sb << "float" << kBaseTextureTypes[tt].coordCount << " gradX, ";
                sb << "float" << kBaseTextureTypes[tt].coordCount << " gradY";
                sb << ");\n";

                if( baseShape != TextureFlavor::Shape::ShapeCube )
                {
                    sb << "__target_intrinsic(glsl, \"textureGradOffset($p, $2, $3, $4, $5)\")\n";
//                    sb << "__intrinsic_op(sampleGrad)\n";
                    sb << "T SampleGrad(SamplerState s, ";
                    sb << "float" << kBaseTextureTypes[tt].coordCount + isArray << " location, ";
                    sb << "float" << kBaseTextureTypes[tt].coordCount << " gradX, ";
                    sb << "float" << kBaseTextureTypes[tt].coordCount << " gradY, ";
                    sb << "constexpr int" << kBaseTextureTypes[tt].coordCount << " offset);\n";
                }

                // `SampleLevel`

                sb << "__target_intrinsic(glsl, \"textureLod($p, $2, $3)\")\n";
                sb << "T SampleLevel(SamplerState s, ";
                sb << "float" << kBaseTextureTypes[tt].coordCount + isArray << " location, ";
                sb << "float level);\n";

                if( baseShape != TextureFlavor::Shape::ShapeCube )
                {
                    sb << "__target_intrinsic(glsl, \"textureLodOffset($p, $2, $3, $4)\")\n";
                    sb << "T SampleLevel(SamplerState s, ";
                    sb << "float" << kBaseTextureTypes[tt].coordCount + isArray << " location, ";
                    sb << "float level, ";
                    sb << "constexpr int" << kBaseTextureTypes[tt].coordCount << " offset);\n";
                }
            }

            sb << "\n};\n";

            // `Gather*()` operations are handled via an `extension` declaration,
            // because this lets us capture the element type of the texture.
            //
            // TODO: longer-term there should be something like a `TextureElementType`
            // interface, that both scalars and vectors implement, that then exposes
            // a `Scalar` associated type, and `Gather` can return `vector<T.Scalar, 4>`.
            //
            static const struct {
                char const* genericPrefix;
                char const* elementType;
            } kGatherExtensionCases[] = {
                { "__generic<T, let N : int>", "vector<T,N>" },

                // TODO: need a case here for scalars `T`, but also
                // need to ensure that case doesn't accidentally match
                // for `T = vector<...>`, which requires actual checking
                // of constraints on generic parameters.
            };
            for(auto cc : kGatherExtensionCases)
            {
                // TODO: this should really be an `if` around the entire `Gather` logic
                if (isMultisample) break;

                EMIT_LINE_DIRECTIVE();
                sb << cc.genericPrefix << " __extension ";
                sb << kBaseTextureAccessLevels[accessLevel].name;
                sb << name;
                if (isArray) sb << "Array";
                sb << "<" << cc.elementType << " >";
                sb << "\n{\n";


                // `Gather`
                // (tricky because it returns a 4-vector of the element type
                // of the texture components...)
                //
                // TODO: is it actually correct to restrict these so that, e.g.,
                // `GatherAlpha()` isn't allowed on `Texture2D<float3>` because
                // it nominally doesn't have an alpha component?
                static const struct {
                    int componentIndex;
                    char const* componentName;
                } kGatherComponets[] = {
                    { 0, "" },
                    { 0, "Red" },
                    { 1, "Green" },
                    { 2, "Blue" },
                    { 3, "Alpha" },
                };

                for(auto kk : kGatherComponets)
                {
                    auto componentIndex = kk.componentIndex;
                    auto componentName = kk.componentName;

                    EMIT_LINE_DIRECTIVE();

                    sb << "__target_intrinsic(glsl, \"textureGather($p, $2, " << componentIndex << ")\")\n";
                    sb << "vector<T, 4> Gather" << componentName << "(SamplerState s, ";
                    sb << "float" << kBaseTextureTypes[tt].coordCount << " location);\n";

                    EMIT_LINE_DIRECTIVE();
                    sb << "__target_intrinsic(glsl, \"textureGatherOffset($p, $2, $3, " << componentIndex << ")\")\n";
                    sb << "vector<T, 4> Gather" << componentName << "(SamplerState s, ";
                    sb << "float" << kBaseTextureTypes[tt].coordCount << " location, ";
                    sb << "constexpr int" << kBaseTextureTypes[tt].coordCount << " offset);\n";

                    EMIT_LINE_DIRECTIVE();
                    sb << "vector<T, 4> Gather" << componentName << "(SamplerState s, ";
                    sb << "float" << kBaseTextureTypes[tt].coordCount << " location, ";
                    sb << "constexpr int" << kBaseTextureTypes[tt].coordCount << " offset, ";
                    sb << "out uint status);\n";

                    EMIT_LINE_DIRECTIVE();
                    sb << "__target_intrinsic(glsl, \"textureGatherOffsets($p, $2, int" << kBaseTextureTypes[tt].coordCount << "[]($3, $4, $5, $6), " << componentIndex << ")\")\n";
                    sb << "vector<T, 4> Gather" << componentName << "(SamplerState s, ";
                    sb << "float" << kBaseTextureTypes[tt].coordCount << " location, ";
                    sb << "int" << kBaseTextureTypes[tt].coordCount << " offset1, ";
                    sb << "int" << kBaseTextureTypes[tt].coordCount << " offset2, ";
                    sb << "int" << kBaseTextureTypes[tt].coordCount << " offset3, ";
                    sb << "int" << kBaseTextureTypes[tt].coordCount << " offset4);\n";

                    EMIT_LINE_DIRECTIVE();
                    sb << "vector<T, 4> Gather" << componentName << "(SamplerState s, ";
                    sb << "float" << kBaseTextureTypes[tt].coordCount << " location, ";
                    sb << "int" << kBaseTextureTypes[tt].coordCount << " offset1, ";
                    sb << "int" << kBaseTextureTypes[tt].coordCount << " offset2, ";
                    sb << "int" << kBaseTextureTypes[tt].coordCount << " offset3, ";
                    sb << "int" << kBaseTextureTypes[tt].coordCount << " offset4, ";
                    sb << "out uint status);\n";
                }

                EMIT_LINE_DIRECTIVE();
                sb << "\n}\n";
            }
        }
    }
}


for (auto op : unaryOps)
{
    for (auto type : kBaseTypes)
    {
        if ((type.flags & op.flags) == 0)
            continue;

        char const* fixity = (op.flags & POSTFIX) != 0 ? "__postfix " : "__prefix ";
        char const* qual = (op.flags & ASSIGNMENT) != 0 ? "in out " : "";

        // scalar version
        sb << fixity;
        sb << "__intrinsic_op(" << int(op.opCode) << ") " << type.name << " operator" << op.opName << "(" << qual << type.name << " value);\n";

        // vector version
        sb << "__generic<let N : int> ";
        sb << fixity;
        sb << "__intrinsic_op(" << int(op.opCode) << ") vector<" << type.name << ",N> operator" << op.opName << "(" << qual << "vector<" << type.name << ",N> value);\n";

        // matrix version
        sb << "__generic<let N : int, let M : int> ";
        sb << fixity;
        sb << "__intrinsic_op(" << int(op.opCode) << ") matrix<" << type.name << ",N,M> operator" << op.opName << "(" << qual << "matrix<" << type.name << ",N,M> value);\n";
    }
}

for (auto op : binaryOps)
{
    for (auto type : kBaseTypes)
    {
        if ((type.flags & op.flags) == 0)
            continue;

        char const* leftType = type.name;
        char const* rightType = leftType;
        char const* resultType = leftType;

        if (op.flags & COMPARISON) resultType = "bool";

        char const* leftQual = "";
        if(op.flags & ASSIGNMENT) leftQual = "in out ";

        // TODO: handle `SHIFT`

        // scalar version
        sb << "__intrinsic_op(" << int(op.opCode) << ") " << resultType << " operator" << op.opName << "(" << leftQual << leftType << " left, " << rightType << " right);\n";

        // vector version
        sb << "__generic<let N : int> ";
        sb << "__intrinsic_op(" << int(op.opCode) << ") vector<" << resultType << ",N> operator" << op.opName << "(" << leftQual << "vector<" << leftType << ",N> left, vector<" << rightType << ",N> right);\n";

        // matrix version

        // skip matrix-matrix multiply operations here, so that GLSL doesn't see them
        switch (op.opCode)
        {
        case kIROp_Mul:
        case kIRPseudoOp_MulAssign:
            break;

        default:
            sb << "__generic<let N : int, let M : int> ";
            sb << "__intrinsic_op(" << int(op.opCode) << ") matrix<" << resultType << ",N,M> operator" << op.opName << "(" << leftQual << "matrix<" << leftType << ",N,M> left, matrix<" << rightType << ",N,M> right);\n";
            break;
        }

        // We are going to go ahead and explicitly define combined
        // operations for the scalar-op-vector, etc. cases, rather
        // than rely on promotion rules.

        // scalar-vector and scalar-matrix
        if (!(op.flags & ASSIGNMENT))
        {
            sb << "__generic<let N : int> ";
            sb << "__intrinsic_op(" << int(op.opCode) << ") vector<" << resultType << ",N> operator" << op.opName << "(" << leftQual << leftType << " left, vector<" << rightType << ",N> right);\n";

            sb << "__generic<let N : int, let M : int> ";
            sb << "__intrinsic_op(" << int(op.opCode) << ") matrix<" << resultType << ",N,M> operator" << op.opName << "(" << leftQual << leftType << " left, matrix<" << rightType << ",N,M> right);\n";
        }

        // vector-scalar and matrix-scalar
        sb << "__generic<let N : int> ";
        sb << "__intrinsic_op(" << int(op.opCode) << ") vector<" << resultType << ",N> operator" << op.opName << "(" << leftQual << "vector<" << leftType << ",N> left, " << rightType << " right);\n";

        sb << "__generic<let N : int, let M : int> ";
        sb << "__intrinsic_op(" << int(op.opCode) << ") matrix<" << resultType << ",N,M> operator" << op.opName << "(" << leftQual << "matrix<" << leftType << ",N,M> left, " << rightType << " right);\n";
    }
}
SLANG_RAW("\n")
SLANG_RAW("\n")
SLANG_RAW("// Operators to apply to `enum` types\n")
SLANG_RAW("\n")
SLANG_RAW("__generic<E : __EnumType>\n")
SLANG_RAW("__intrinsic_op(")
SLANG_SPLICE(kIROp_Eql
)
SLANG_RAW(")\n")
SLANG_RAW("bool operator==(E left, E right);\n")
SLANG_RAW("\n")
SLANG_RAW("// Statement Attributes\n")
SLANG_RAW("\n")
SLANG_RAW("__attributeTarget(LoopStmt)\n")
SLANG_RAW("attribute_syntax [unroll(count: int = 0)]   : UnrollAttribute;\n")
SLANG_RAW("\n")
SLANG_RAW("__attributeTarget(LoopStmt)\n")
SLANG_RAW("attribute_syntax [loop]                 : LoopAttribute;\n")
SLANG_RAW("\n")
SLANG_RAW("__attributeTarget(LoopStmt)\n")
SLANG_RAW("attribute_syntax [fastopt]              : FastOptAttribute;\n")
SLANG_RAW("\n")
SLANG_RAW("__attributeTarget(LoopStmt)\n")
SLANG_RAW("attribute_syntax [allow_uav_condition]  : AllowUAVConditionAttribute;\n")
SLANG_RAW("\n")
SLANG_RAW("__attributeTarget(IfStmt)\n")
SLANG_RAW("attribute_syntax [flatten]              : FlattenAttribute;\n")
SLANG_RAW("\n")
SLANG_RAW("__attributeTarget(IfStmt)\n")
SLANG_RAW("__attributeTarget(SwitchStmt)\n")
SLANG_RAW("attribute_syntax [branch]               : BranchAttribute;\n")
SLANG_RAW("\n")
SLANG_RAW("__attributeTarget(SwitchStmt)\n")
SLANG_RAW("attribute_syntax [forcecase]            : ForceCaseAttribute;\n")
SLANG_RAW("\n")
SLANG_RAW("__attributeTarget(SwitchStmt)\n")
SLANG_RAW("attribute_syntax [call]                 : CallAttribute;\n")
SLANG_RAW("\n")
SLANG_RAW("// Entry-point Attributes\n")
SLANG_RAW("\n")
SLANG_RAW("// All Stages\n")
SLANG_RAW("__attributeTarget(FuncDecl)\n")
SLANG_RAW("attribute_syntax [shader(stage)]    : EntryPointAttribute;\n")
SLANG_RAW("\n")
SLANG_RAW("// Hull Shader\n")
SLANG_RAW("__attributeTarget(FuncDecl)\n")
SLANG_RAW("attribute_syntax [maxtessfactor(factor: float)]     : MaxTessFactorAttribute;\n")
SLANG_RAW("\n")
SLANG_RAW("__attributeTarget(FuncDecl)\n")
SLANG_RAW("attribute_syntax [outputcontrolpoints(count: int)]  : OutputControlPointsAttribute;\n")
SLANG_RAW("\n")
SLANG_RAW("__attributeTarget(FuncDecl)\n")
SLANG_RAW("attribute_syntax [outputtopology(topology)]         : OuptutTopologyAttribute;\n")
SLANG_RAW("\n")
SLANG_RAW("__attributeTarget(FuncDecl)\n")
SLANG_RAW("attribute_syntax [partitioning(mode)]               : PartitioningAttribute;\n")
SLANG_RAW("\n")
SLANG_RAW("__attributeTarget(FuncDecl)\n")
SLANG_RAW("attribute_syntax [patchconstantfunc(name)]          : PatchConstantFuncAttribute;\n")
SLANG_RAW("\n")
SLANG_RAW("// Hull/Domain Shader\n")
SLANG_RAW("__attributeTarget(FuncDecl)\n")
SLANG_RAW("attribute_syntax [domain(domain)]   : DomainAttribute;\n")
SLANG_RAW("\n")
SLANG_RAW("// Geometry Shader\n")
SLANG_RAW("__attributeTarget(FuncDecl)\n")
SLANG_RAW("attribute_syntax [maxvertexcount(count: int)]   : MaxVertexCountAttribute;\n")
SLANG_RAW("\n")
SLANG_RAW("__attributeTarget(FuncDecl)\n")
SLANG_RAW("attribute_syntax [instance(count: int)]         : InstanceAttribute;\n")
SLANG_RAW("\n")
SLANG_RAW("// Fragment (\"Pixel\") Shader\n")
SLANG_RAW("__attributeTarget(FuncDecl)\n")
SLANG_RAW("attribute_syntax [earlydepthstencil]    : EarlyDepthStencilAttribute;\n")
SLANG_RAW("\n")
SLANG_RAW("// Compute Shader\n")
SLANG_RAW("__attributeTarget(FuncDecl)\n")
SLANG_RAW("attribute_syntax [numthreads(x: int, y: int = 1, z: int = 1)]   : NumThreadsAttribute;\n")
