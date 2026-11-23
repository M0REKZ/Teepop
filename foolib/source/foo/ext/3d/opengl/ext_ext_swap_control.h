typedef GLboolean (GLFUNC *PFNSWAPINTERVALEXTPROC)( GLint GLinterval);
typedef GLint (GLFUNC *PFNGETSWAPINTERVALEXTPROC)( void);
extern PFNSWAPINTERVALEXTPROC glextSwapIntervalEXT;
extern PFNGETSWAPINTERVALEXTPROC glextGetSwapIntervalEXT;


bool GLExtInit_ext_swap_control();

