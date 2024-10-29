#pragma once
#include<fstream>
//コンパイル済みシェーダを読み込むファイルバッファ
class BIN_FILE12 {
public:
    BIN_FILE12(const char* fileName) :Succeeded(false)
    {
        std::ifstream ifs(fileName, std::ios::binary);
        if (ifs.fail()) {
            return;
        }
        Succeeded = true;
        std::istreambuf_iterator<char> first(ifs);
        std::istreambuf_iterator<char> last;
        Buffer.assign(first, last);
        ifs.close();
    }
    bool succeeded() const
    {
        return Succeeded;
    }
    unsigned char* code() const
    {
        char* p = const_cast<char*>(Buffer.data());
        return reinterpret_cast<unsigned char*>(p);
    }
    size_t size() const
    {
        return Buffer.size();
    }
private:
    std::string Buffer;
    bool Succeeded;
};
