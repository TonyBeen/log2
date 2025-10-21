/*************************************************************************
    > File Name: benchmark.cc
    > Author: hsz
    > Brief:
    > Created Time: 2025年10月21日 星期二 16时00分46秒
 ************************************************************************/

#include <utils/elapsed_time.h>
#include <log2/log.h>

#define LOG_TAG "bench"

int main(int argc, char **argv)
{
    eular::ElapsedTime elapsedTime(eular::ElapsedTimeType::NANOSECOND);

    int32_t cycle = 1000;
    for (int32_t i = 0; i < cycle; i++) {
        elapsedTime.start();
        LOGI("i = %.2f", (double)i);
        elapsedTime.stop();
    }

    LOGD("elapsedTime = %.2f ns", (double)elapsedTime.elapsedTime() / cycle);
    return 0;
}
