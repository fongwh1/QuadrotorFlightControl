#ifndef __SENSOR_H
#define __SENSOR_H

#define LOW_PASS_FACTOR	0.75
#define LOW_PASS(previous,now) (float)((float)(now*LOW_PASS_FACTOR)+(float)((1-LOW_PASS_FACTOR)*previous))

void sensor_task(void * pvParameters);

#endif
