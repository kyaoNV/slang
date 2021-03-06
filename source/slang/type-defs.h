// type-defs.h

// Syntax class definitions for types.

// The type of a reference to an overloaded name
SYNTAX_CLASS(OverloadGroupType, ExpressionType)
RAW(
public:
    virtual String ToString() override;

protected:
    virtual bool EqualsImpl(ExpressionType * type) override;
    virtual ExpressionType* CreateCanonicalType() override;
    virtual int GetHashCode() override;
)
END_SYNTAX_CLASS()

// The type of an initializer-list expression (before it has
// been coerced to some other type)
SYNTAX_CLASS(InitializerListType, ExpressionType)
RAW(
    virtual String ToString() override;

protected:
    virtual bool EqualsImpl(ExpressionType * type) override;
    virtual ExpressionType* CreateCanonicalType() override;
    virtual int GetHashCode() override;
)
END_SYNTAX_CLASS()

// The type of an expression that was erroneous
SYNTAX_CLASS(ErrorType, ExpressionType)
RAW(
public:
    virtual String ToString() override;

protected:
    virtual bool EqualsImpl(ExpressionType * type) override;
    virtual ExpressionType* CreateCanonicalType() override;
    virtual int GetHashCode() override;
)
END_SYNTAX_CLASS()

// A type that takes the form of a reference to some declaration
SYNTAX_CLASS(DeclRefType, ExpressionType)
    DECL_FIELD(DeclRef<Decl>, declRef)

RAW(
    virtual String ToString() override;
    virtual RefPtr<Val> SubstituteImpl(Substitutions* subst, int* ioDiff) override;

    static DeclRefType* Create(DeclRef<Decl> declRef);

protected:
    DeclRefType()
    {}
    DeclRefType(DeclRef<Decl> declRef)
        : declRef(declRef)
    {}
    virtual int GetHashCode() override;
    virtual bool EqualsImpl(ExpressionType * type) override;
    virtual ExpressionType* CreateCanonicalType() override;
)
END_SYNTAX_CLASS()

// Base class for types that can be used in arithmetic expressions
SYNTAX_CLASS(ArithmeticExpressionType, DeclRefType)
RAW(
    virtual BasicExpressionType* GetScalarType() = 0;
)
END_SYNTAX_CLASS()

SYNTAX_CLASS(BasicExpressionType, ArithmeticExpressionType)

    FIELD(BaseType, BaseType)

RAW(
    BasicExpressionType()
    {
        BaseType = Slang::BaseType::Int;
    }
    BasicExpressionType(Slang::BaseType baseType)
    {
        BaseType = baseType;
    }
    virtual Slang::String ToString() override;
protected:
    virtual BasicExpressionType* GetScalarType() override;
    virtual bool EqualsImpl(ExpressionType * type) override;
    virtual ExpressionType* CreateCanonicalType() override;
)
END_SYNTAX_CLASS()


SYNTAX_CLASS(TextureTypeBase, DeclRefType)
    // The type that results from fetching an element from this texture
    SYNTAX_FIELD(RefPtr<ExpressionType>, elementType)

    // Bits representing the kind of texture type we are looking at
    // (e.g., `Texture2DMS` vs. `TextureCubeArray`)
    RAW(typedef uint16_t Flavor;)
    FIELD(Flavor, flavor)

RAW(
    enum
    {
        // Mask for the overall "shape" of the texture
        ShapeMask		= SLANG_RESOURCE_BASE_SHAPE_MASK,

        // Flag for whether the shape has "array-ness"
        ArrayFlag		= SLANG_TEXTURE_ARRAY_FLAG,

        // Whether or not the texture stores multiple samples per pixel
        MultisampleFlag	= SLANG_TEXTURE_MULTISAMPLE_FLAG,

        // Whether or not this is a shadow texture
        //
        // TODO(tfoley): is this even meaningful/used?
        // ShadowFlag		= 0x80, 
    };

    enum Shape : uint8_t
    {
        Shape1D			= SLANG_TEXTURE_1D,
        Shape2D			= SLANG_TEXTURE_2D,
        Shape3D			= SLANG_TEXTURE_3D,
        ShapeCube		= SLANG_TEXTURE_CUBE,
        ShapeBuffer     = SLANG_TEXTURE_BUFFER,

        Shape1DArray	= Shape1D | ArrayFlag,
        Shape2DArray	= Shape2D | ArrayFlag,
        // No Shape3DArray
        ShapeCubeArray	= ShapeCube | ArrayFlag,
    };
            

    Shape GetBaseShape() const { return Shape(flavor & ShapeMask); }
    bool isArray() const { return (flavor & ArrayFlag) != 0; }
    bool isMultisample() const { return (flavor & MultisampleFlag) != 0; }
//            bool isShadow() const { return (flavor & ShadowFlag) != 0; }

    SlangResourceShape getShape() const { return flavor & 0xFF; }
    SlangResourceAccess getAccess() const { return (flavor >> 8) & 0xFF; }

    TextureTypeBase(
        Flavor flavor,
        RefPtr<ExpressionType> elementType)
        : elementType(elementType)
        , flavor(flavor)
    {}
)
END_SYNTAX_CLASS()

SYNTAX_CLASS(TextureType, TextureTypeBase)
RAW(
    TextureType(
        Flavor flavor,
        RefPtr<ExpressionType> elementType)
        : TextureTypeBase(flavor, elementType)
    {}
)
END_SYNTAX_CLASS()

// This is a base type for texture/sampler pairs,
// as they exist in, e.g., GLSL
SYNTAX_CLASS(TextureSamplerType, TextureTypeBase)
RAW(
    TextureSamplerType(
        Flavor flavor,
        RefPtr<ExpressionType> elementType)
        : TextureTypeBase(flavor, elementType)
    {}
)
END_SYNTAX_CLASS()

// This is a base type for `image*` types, as they exist in GLSL
SYNTAX_CLASS(GLSLImageType, TextureTypeBase)
RAW(
    GLSLImageType(
        Flavor flavor,
        RefPtr<ExpressionType> elementType)
        : TextureTypeBase(flavor, elementType)
    {}
)
END_SYNTAX_CLASS()

SYNTAX_CLASS(SamplerStateType, DeclRefType)

    // What flavor of sampler state is this
    RAW(enum class Flavor : uint8_t
    {
        SamplerState,
        SamplerComparisonState,
    };)
    FIELD(Flavor, flavor)
END_SYNTAX_CLASS()

// Other cases of generic types known to the compiler
SYNTAX_CLASS(BuiltinGenericType, DeclRefType)
    SYNTAX_FIELD(RefPtr<ExpressionType>, elementType)
END_SYNTAX_CLASS()

// Types that behave like pointers, in that they can be
// dereferenced (implicitly) to access members defined
// in the element type.
SIMPLE_SYNTAX_CLASS(PointerLikeType, BuiltinGenericType)

// Generic types used in existing Slang code
// TODO(tfoley): check that these are actually working right...
SIMPLE_SYNTAX_CLASS(PatchType, PointerLikeType)
SIMPLE_SYNTAX_CLASS(StorageBufferType, BuiltinGenericType)
SIMPLE_SYNTAX_CLASS(UniformBufferType, PointerLikeType)
SIMPLE_SYNTAX_CLASS(PackedBufferType, BuiltinGenericType)

// HLSL buffer-type resources

SIMPLE_SYNTAX_CLASS(HLSLBufferType, BuiltinGenericType)
SIMPLE_SYNTAX_CLASS(HLSLRWBufferType, BuiltinGenericType)
SIMPLE_SYNTAX_CLASS(HLSLStructuredBufferType, BuiltinGenericType)
SIMPLE_SYNTAX_CLASS(HLSLRWStructuredBufferType, BuiltinGenericType)

SIMPLE_SYNTAX_CLASS(UntypedBufferResourceType, DeclRefType)
SIMPLE_SYNTAX_CLASS(HLSLByteAddressBufferType, UntypedBufferResourceType)
SIMPLE_SYNTAX_CLASS(HLSLRWByteAddressBufferType, UntypedBufferResourceType)

SIMPLE_SYNTAX_CLASS(HLSLAppendStructuredBufferType, BuiltinGenericType)
SIMPLE_SYNTAX_CLASS(HLSLConsumeStructuredBufferType, BuiltinGenericType)

SYNTAX_CLASS(HLSLPatchType, DeclRefType)
RAW(
    ExpressionType* getElementType();
    IntVal*         getElementCount();
)
END_SYNTAX_CLASS()

SIMPLE_SYNTAX_CLASS(HLSLInputPatchType, HLSLPatchType)
SIMPLE_SYNTAX_CLASS(HLSLOutputPatchType, HLSLPatchType)

// HLSL geometry shader output stream types

SIMPLE_SYNTAX_CLASS(HLSLStreamOutputType, BuiltinGenericType)
SIMPLE_SYNTAX_CLASS(HLSLPointStreamType, HLSLStreamOutputType)
SIMPLE_SYNTAX_CLASS(HLSLLineStreamType, HLSLStreamOutputType)
SIMPLE_SYNTAX_CLASS(HLSLTriangleStreamType, HLSLStreamOutputType)

//
SIMPLE_SYNTAX_CLASS(GLSLInputAttachmentType, DeclRefType)

// Base class for types used when desugaring parameter block
// declarations, includeing HLSL `cbuffer` or GLSL `uniform` blocks.
SIMPLE_SYNTAX_CLASS(ParameterBlockType, PointerLikeType)

SIMPLE_SYNTAX_CLASS(UniformParameterBlockType, ParameterBlockType)
SIMPLE_SYNTAX_CLASS(VaryingParameterBlockType, ParameterBlockType)

// Type for HLSL `cbuffer` declarations, and `ConstantBuffer<T>`
// ALso used for GLSL `uniform` blocks.
SIMPLE_SYNTAX_CLASS(ConstantBufferType, UniformParameterBlockType)

// Type for HLSL `tbuffer` declarations, and `TextureBuffer<T>`
SIMPLE_SYNTAX_CLASS(TextureBufferType, UniformParameterBlockType)

// Type for GLSL `in` and `out` blocks
SIMPLE_SYNTAX_CLASS(GLSLInputParameterBlockType, VaryingParameterBlockType)
SIMPLE_SYNTAX_CLASS(GLSLOutputParameterBlockType, VaryingParameterBlockType)

// Type for GLLSL `buffer` blocks
SIMPLE_SYNTAX_CLASS(GLSLShaderStorageBufferType, UniformParameterBlockType)

SYNTAX_CLASS(ArrayExpressionType, ExpressionType)
    SYNTAX_FIELD(RefPtr<ExpressionType>, BaseType)
    SYNTAX_FIELD(RefPtr<IntVal>, ArrayLength)

RAW(
    virtual Slang::String ToString() override;
protected:
    virtual bool EqualsImpl(ExpressionType * type) override;
    virtual ExpressionType* CreateCanonicalType() override;
    virtual int GetHashCode() override;
)
END_SYNTAX_CLASS()

// The "type" of an expression that resolves to a type.
// For example, in the expression `float(2)` the sub-expression,
// `float` would have the type `TypeType(float)`.
SYNTAX_CLASS(TypeType, ExpressionType)
    // The type that this is the type of...
    SYNTAX_FIELD(RefPtr<ExpressionType>, type)

RAW(
public:
    TypeType(RefPtr<ExpressionType> type)
        : type(type)
    {}

    virtual String ToString() override;

protected:
    virtual bool EqualsImpl(ExpressionType * type) override;
    virtual ExpressionType* CreateCanonicalType() override;
    virtual int GetHashCode() override;
)
END_SYNTAX_CLASS()

// A vector type, e.g., `vector<T,N>`
SYNTAX_CLASS(VectorExpressionType, ArithmeticExpressionType)

    // The type of vector elements.
    // As an invariant, this should be a basic type or an alias.
    SYNTAX_FIELD(RefPtr<ExpressionType>, elementType)

    // The number of elements
    SYNTAX_FIELD(RefPtr<IntVal>, elementCount)

RAW(
    virtual String ToString() override;

protected:
    virtual BasicExpressionType* GetScalarType() override;
)
END_SYNTAX_CLASS()

// A matrix type, e.g., `matrix<T,R,C>`
SYNTAX_CLASS(MatrixExpressionType, ArithmeticExpressionType)
RAW(
    ExpressionType* getElementType();
    IntVal*         getRowCount();
    IntVal*         getColumnCount();


    virtual String ToString() override;

protected:
    virtual BasicExpressionType* GetScalarType() override;
)
END_SYNTAX_CLASS()

// A type alias of some kind (e.g., via `typedef`)
SYNTAX_CLASS(NamedExpressionType, ExpressionType)
    DECL_FIELD(DeclRef<TypeDefDecl>, declRef)

RAW(
    NamedExpressionType(DeclRef<TypeDefDecl> declRef)
        : declRef(declRef)
    {}


    virtual String ToString() override;

protected:
    virtual bool EqualsImpl(ExpressionType * type) override;
    virtual ExpressionType* CreateCanonicalType() override;
    virtual int GetHashCode() override;
)
END_SYNTAX_CLASS()

// Function types are currently used for references to symbols that name
// either ordinary functions, or "component functions."
// We do not directly store a representation of the type, and instead
// use a reference to the symbol to stand in for its logical type
SYNTAX_CLASS(FuncType, ExpressionType)
    DECL_FIELD(DeclRef<CallableDecl>, declRef)

RAW(
    virtual String ToString() override;
protected:
    virtual bool EqualsImpl(ExpressionType * type) override;
    virtual ExpressionType* CreateCanonicalType() override;
    virtual int GetHashCode() override;
)
END_SYNTAX_CLASS()

// The "type" of an expression that names a generic declaration.
SYNTAX_CLASS(GenericDeclRefType, ExpressionType)

    DECL_FIELD(DeclRef<GenericDecl>, declRef)

    RAW(
    GenericDeclRefType(DeclRef<GenericDecl> declRef)
        : declRef(declRef)
    {}

    
    DeclRef<GenericDecl> const& GetDeclRef() const { return declRef; }

    virtual String ToString() override;

protected:
    virtual bool EqualsImpl(ExpressionType * type) override;
    virtual int GetHashCode() override;
    virtual ExpressionType* CreateCanonicalType() override;
)
END_SYNTAX_CLASS()

