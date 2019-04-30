#ifdef CVP_EXPORTS
#	define CVP_DECLSPEC __declspec(dllexport)
#else
#	define CVP_DECLSPEC __declspec(dllimport)
#endif