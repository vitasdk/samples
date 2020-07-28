#include <psp2/kernel/processmgr.h>
#include <psp2/sysmodule.h>
#include <psp2/ctrl.h>
#include <psp2/json.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <debugScreen.h>

using namespace sce::Json;

class Allocator : public MemAllocator
{
public:
    Allocator() : s(0) {}

    virtual void* allocateMemory(size_t size, void *unk) override
    {
        this->s -= size;
        if(this->s == 0)
            return nullptr;
        return malloc(size);
    }
    virtual void freeMemory(void *ptr, void *unk) override
    {
        free(ptr);
    }   
private:
    int s;
};

#define printf psvDebugScreenPrintf

char* DisplayTestFile()
{
    FILE* fd = fopen("app0:test.json", "rb");
    char* buf; int size;
    fseek(fd, 0, SEEK_END);
    size = ftell(fd);
    fseek(fd, 0, SEEK_SET);

    buf = (char*)malloc(size);
    fread(buf, size, 1, fd);
    fclose(fd);

    printf("Input Json Data: \n%s\n", buf);

    return buf;           
}

int main()
{
    psvDebugScreenInit();

    // Load the Json module into memory.
    sceSysmoduleLoadModule(SCE_SYSMODULE_JSON);

    // Create a MemAllocator derivative.
    Allocator* alloc = new Allocator();

    InitParameter params;
    params.allocator = alloc;
    params.unk_0x8 = 1; // Since we're using Parser::parse(Value&, const char*) we need to set this to something higher than zero.

    Initializer init = Initializer();
    int ret = init.initialize(&params); // Initialization at last!

    char* buf = DisplayTestFile();

    Value val = Value(); // Creating our Root value. 
    Parser::parse(val, "app0:test.json"); // Parse test.json into val.

    printf("\nIterating over parsed data\n\n");
    /**
     * This loop will iterate through the child values of the root value, and ouput their individual values.
     */
    for(int i = 0; i < val.count(); i++)
    {
        String s = String();
        const Value& v = val.getValue(i);
        printf("\nChild %d\n", i);

        switch(v.getType())
        {
            case ValueType::BoolValue:
            case ValueType::IntValue:
            case ValueType::UIntValue:
            case ValueType::RealValue:
                v.toString(s);
                printf("Value: %s\n", s.c_str());
                break;
            case ValueType::StringValue:
                printf("Value: %s\n", v.getString().c_str());
                break;
            case ValueType::ArrayValue:
                {
                    printf("Child %d is an Array. Iterating though values.\n", i);
                    const Array& arr = v.getArray();
                    int j = 0;
                    for (Array::iterator it = arr.begin(); it != arr.end(); it++) // Iterate over child values.
                    {
                        Value& va = *it;
                        switch (va.getType())
                        {
                        case ValueType::BoolValue:
                        case ValueType::IntValue:
                        case ValueType::UIntValue:
                        case ValueType::RealValue:
                            va.toString(s);
                            printf("Value %d: %s\n", j, s.c_str());
                            break;
                        case ValueType::StringValue:
                            printf("Value %d: %s\n", j, va.getString().c_str());
                            break;
                        default:
                            printf("Value is null\n");
                        }
                        s.clear();
                        ++j;
                    }
                }
                break;
            case ValueType::ObjectValue: 
                {
                    printf("Child %d is an Object. Iterating though values.\n\n", i);
                    const Object& obj = v.getObject();
                    for (Object::Pair& pair : obj) // Iterate over child values.
                    {
                        printf("Key: %s\nType: %d\n", pair.key.c_str(), pair.value.getType());
                        switch (pair.value.getType())
                        {
                        case ValueType::BoolValue:
                            printf("Value: %d\n", pair.value.getBoolean());
                            break;
                        case ValueType::IntValue:
                            printf("Value: %lld\n", (int)pair.value.getInteger());
                            break;
                        case ValueType::UIntValue:
                            printf("Value: %llu\n", pair.value.getUInteger());
                            break;
                        case ValueType::RealValue:
                            printf("Value: %f\n", pair.value.getReal());
                            break;
                        case ValueType::StringValue:
                            printf("Value: %s\n", pair.value.getString().c_str());
                            break;
                        default:
                            printf("Value is null\n");
                        }
                    }
                }
                break;
            default:
                printf("Value is null");
        }
    }

    String serialized = String();
    val.serialize(serialized); // Serialize val into parsed. 
    printf("\nSerialized Output Data: \n%s\n", serialized.c_str()); // Display parsed.
    
    int same = strcmp(serialized.c_str(), buf); // Compare the input data to the serialized data.
    if(same < 0 || same > 0)
        printf("The serialized output is not the same as the input.\n");
    else
        printf("The serialized output is the same as the input.\n");

    // Terminate the library.
    init.terminate();
    // Unload the library's module.
    sceSysmoduleUnloadModule(SCE_SYSMODULE_JSON);

    printf("\nPress X to exit.");

    sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);
    SceCtrlData pad;
    memset(&pad, 0, sizeof(SceCtrlData));
    for(;;)
    {
        sceCtrlPeekBufferPositive(0, &pad, 1);

        if(pad.buttons & SCE_CTRL_CROSS)
            break;
    }

    sceKernelExitProcess(0);
    
    return 0;
}
