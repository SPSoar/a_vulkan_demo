
_SSR_STUDIO_BEGIN

namespace 
{

static const SVEC3 SCCV_Box_Vertices[] = {
       {-1.f,  1.f, -1.f}, // front face
       { 1.f, -1.f, -1.f},
       {-1.f, -1.f, -1.f},
       { 1.f,  1.f, -1.f},

       { 1.f, -1.f, -1.f}, // right side face
       { 1.f,  1.f,  1.f},
       { 1.f, -1.f,  1.f},
       { 1.f,  1.f, -1.f},

       {-1.f,  1.f,  1.f}, // left side face
       {-1.f, -1.f, -1.f},
       {-1.f, -1.f,  1.f},
       {-1.f,  1.f, -1.f},

       { 1.f,  1.f,  1.f}, // back face
       {-1.f, -1.f,  1.f},
       { 1.f, -1.f,  1.f},
       {-1.f,  1.f,  1.f},

       {-1.f,  1.f, -1.f}, // top face
       { 1.f,  1.f,  1.f},
       { 1.f,  1.f, -1.f},
       {-1.f,  1.f,  1.f},

       { 1.f, -1.f,  1.f}, // bottom face
       {-1.f, -1.f, -1.f},
       { 1.f, -1.f, -1.f},
       {-1.f, -1.f,  1.f},
    };

static const SVEC2 SCCV_Box_Uvs[] = {
    { 0.0f, 0.0f }, // front face
    { 1.0f, 1.0f },
    { 0.0f, 1.0f },
    { 1.0f, 0.0f },
                 
    { 0.0f, 1.0f }, // right side face
    { 1.0f, 0.0f },
    { 1.0f, 1.0f },
    { 0.0f, 0.0f },

    { 0.0f, 0.0f }, // left side face
    { 1.0f, 1.0f },
    { 0.0f, 1.0f },
    { 1.0f, 0.0f },
                 
    { 0.0f, 0.0f }, // back face
    { 1.0f, 1.0f },
    { 0.0f, 1.0f },
    { 1.0f, 0.0f },

    { 0.0f, 1.0f }, // top face
    { 1.0f, 0.0f },
    { 1.0f, 1.0f },
    { 0.0f, 0.0f },
                 
    { 1.0f, 1.0f }, // bottom face
    { 0.0f, 0.0f },
    { 1.0f, 0.0f },
    { 0.0f, 1.0f },
};

static const SUINT32 SCCV_Box_Indices[] = {
         0,  1,  2,   0,  3,  1, // front face
         4,  5,  6,   4,  7,  5, // left face
         8,  9, 10,   8, 11,  9, // right face
        12, 13, 14,  12, 15, 13, // back face
        16, 17, 18,  16, 19, 17, // top face
        20, 21, 22,  20, 23, 21, // bottom face
        };

}

constexpr const SUINT32 CCV_Box_Vertices_SIZE =  24;
constexpr const SUINT32 CCV_Box_Vertices_BSIZE = cast< SUINT32>(24 * SVEC3::bsize());
constexpr const SUINT32 CCV_Box_Uvs_BSIZE = cast< SUINT32>(24 * SVEC2::bsize());

constexpr const SUINT32 CCV_Box_Indices_SIZE  = sizeof(SCCV_Box_Indices) / sizeof(SCCV_Box_Indices[0]);
constexpr const SUINT32 CCV_Box_Indices_BSIZE  = sizeof(SCCV_Box_Indices);

_SSR_END
