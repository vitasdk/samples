
#ifndef _VITASDK_SAMPLE_MATH_H_
#define _VITASDK_SAMPLE_MATH_H_


#include <malloc.h>


namespace vitasdk {
	namespace sample {
		class Math {
		public:
			void *operator new(unsigned int length){
				return malloc(length);
			}

			void operator delete(void *ptr){
				free(ptr);
			}

			Math(void);
			virtual ~Math();

			int Add(int a, int b);
			int Sub(int a, int b);
			int Mul(int a, int b);
			int Div(int a, int b);
			int Mod(int a, int b);
		};
	}
}


#endif /* _VITASDK_SAMPLE_MATH_H_ */
