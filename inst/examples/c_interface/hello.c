#include <RInside_C.h>

int main() {
    setupRinC();
    evalQuietlyInR("print('Hello, World')");
    teardownRinC();
}
