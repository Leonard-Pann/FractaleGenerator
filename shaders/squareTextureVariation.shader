#version 460

layout(local_size_x = 256) in;

layout(std430, binding = 0) buffer Input
{
    float data[];
};

layout(std430, binding = 1) buffer Output
{
    uint result;
};

shared float partialSum[256]; // shared memory with thread in the same workgroup

uniform int width;
uniform int height;

float getDifferences(int row, int col)
{
    int colM1 = col - 1;
    int colP1 = col + 1;
    int wTRowM1 = width * (row - 1);
    int wTRow = wTRowM1 + width; 
    int wTRowP1 = wTRow + width; 

    float currentGreyscale = data[wTRow + col];
    float sum = abs(currentGreyscale - data[wTRowM1 + colM1]);
    sum += abs(currentGreyscale - data[wTRowM1 + col]);
    sum += abs(currentGreyscale - data[wTRowM1 + colP1]);
    sum += abs(currentGreyscale - data[wTRow + colM1]);
    sum += abs(currentGreyscale - data[wTRow + colP1]);
    sum += abs(currentGreyscale - data[wTRowP1 + colM1]);
    sum += abs(currentGreyscale - data[wTRowP1 + col]);
    sum += abs(currentGreyscale - data[wTRowP1 + colP1]);
    return sum;
}

void main()
{
    int gid = int(gl_GlobalInvocationID.x); // the id or nunber of the thread group (if gid = 784 its the thread 784)
    int lid = int(gl_LocalInvocationID.x); // the id of the tread in the group (between 0 and local_size_x - 1 = 255 here)
    int groupSize = int(gl_WorkGroupSize.x); // size of the workgroup (local_size_x = 256 here)

    float val = 0.0;
    if(gid < width * height)
    {
        int row = gid / width;
        int col = gid % width;

        if(row != 0 && row != height - 1 && col != 0 && col != width - 1)
        {
            val = getDifferences(row, col);
        }
    }

    partialSum[lid] = val;

    memoryBarrierShared(); // wait all write in the workgroup shared memory is visible
    barrier(); // wait until all threads in the same workgroup reach this line

    for (uint s = uint(groupSize) / 2; s > 0; s >>= 1)
    {
        if (lid < s)
        {
            partialSum[lid] += partialSum[lid + s];
        }
        memoryBarrierShared();
        barrier();
    }

    memoryBarrierShared();
    barrier();

    if (lid == 0) 
    {
        double sumFloat = double(partialSum[0]) / 8.0; // sumFloat € [0, local_size_x = 256]
        double nbWorkingGroup = double(width * height) / double(groupSize);
        double maxSumFloat = double(groupSize);
        double factor = 4000000000.0 / (maxSumFloat * nbWorkingGroup);

        uint partilSumUint = uint(sumFloat * factor);
        atomicAdd(result, partilSumUint);
    }
}
