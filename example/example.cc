/*************************************************************************
    > File Name: example.cc
    > Author: hsz
    > Brief:
    > Created Time: 2024年08月12日 星期一 16时49分35秒
 ************************************************************************/

#include <thread>

#include <log2/log.h>
#include <callstack/callstack.h>

#define LOG_TAG "Example"

void frame2()
{
    utils::CallStack stack;
    stack.update(2, 1);

    LOGI("\n%s", stack.to_string().c_str());
}

void frame1()
{
    frame2();
}

int main(int argc, char **argv)
{
    LOGD("-------------->");
    LOGI("-------------->");
    LOGW("-------------->");
    LOGE("-------------->");
    LOGF("-------------->");

    frame1();

    return 0;
}
