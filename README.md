# libepc
[![ci](https://github.com/hyusuk/libepc/actions/workflows/build.yaml/badge.svg)](https://github.com/hyusuk/libepc/actions/workflows/build.yaml)

libepc is an EPC Tag Data encoding library for C++.

## Features
- Supporting encoding/decoding for EPC URI, EPC Tag URI and EPC Binary.
- Accordance with [EPC Tag Data Standard 1.13](https://www.gs1.org/sites/default/files/docs/epc/GS1_EPC_TDS_i1_13.pdf).
- No external dependency except standard libary.

## Supporting EPCs
- [x] GIAI(Global Indivisual Asset Identifier)
- [x] GRAI(Global Returnable Asset Identifier)
- [x] SGLN(GLobal Location Number)
- [x] SGTIN(Serialised GLobal Trade Item Number)
- [x] SSCC(Serial Shipping Container Code)

## Examples

The code below creating a SGTIN object and printing EPC URI, EPC Tag
URI and EPC binary of it.

```cpp
#include "sgtin.h"
#include "status.h"
#include <iostream>

using namespace epc;

int main() {
    SGTIN sgtin;
    Status status;
    std::tie(status, sgtin) = SGTIN::create("0614141", "812345", "6789");
    if (status != Status::kOk) {
        return 1;
    }

    std::cout << "EPC URI: " << sgtin.getURI() << std::endl;
    std::cout << "EPC Tag URI: " << sgtin.getTagURI() << std::endl;
    std::string bin;
    std::tie(status, bin) = sgtin.getBinary();
    if (status != Status::kOk) {
        return 1;
    }
    std::cout << "EPC Binary: " << bin << std::endl;
    return 0;
}
```

Output

```
EPC URI: urn:epc:id:sgtin:0614141.812345.6789
EPC Tag URI: urn:epc:tag:sgtin-96:0.0614141.812345.6789
EPC Binary: 3014257BF7194E4000001A85
```

For more information, See header files under the include/ directory.

## Building
This project supports [CMake](https://cmake.org/) out of the box.

```shell
mkdir -p build && cd build
cmake .. && cmake --build .
```

## Testing

```shell
mkdir -p build && cd build
cmake -DLIBEPC_BUILD_TESTS=ON .. && cmake --build . && ctest --verbose
```


