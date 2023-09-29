
#include "math.h"


__attribute__((__visibility__("default")))
vitasdk::sample::Math::Math(void){
}

__attribute__((__visibility__("default")))
vitasdk::sample::Math::~Math(){
}

__attribute__((__visibility__("default")))
int vitasdk::sample::Math::Add(int a, int b){
	return a + b;
}

__attribute__((__visibility__("default")))
int vitasdk::sample::Math::Sub(int a, int b){
	return a - b;
}

__attribute__((__visibility__("default")))
int vitasdk::sample::Math::Mul(int a, int b){
	return a * b;
}

__attribute__((__visibility__("default")))
int vitasdk::sample::Math::Div(int a, int b){
	return a / b;
}

__attribute__((__visibility__("default")))
int vitasdk::sample::Math::Mod(int a, int b){
	return a % b;
}
