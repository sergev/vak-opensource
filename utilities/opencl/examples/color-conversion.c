/*
 * Web page:
 * http://blog.imgtec.com/powervr/instagram-like-computational-photography-powervr-sgx-gpus
 */

/* Generic 3x3 linear filter kernel. */
__kernel void convolve3x3(__read_only  image2d_t  srcImage,
                          __write_only image2d_t  dstImage,
                          sampler_t               sampler,
                          __constant float        *kVals,
                          float                    normalizationVal)
{
        int2 coords = (int2)(get_global_id(0), get_global_id(1));

        float4 colour;
        float4 colours[9];

        colours[0] = read_imagef(srcImage, sampler, coords + (int2)(-1, -1));
        colours[1] = read_imagef(srcImage, sampler, coords + (int2)( 0, -1));
        colours[2] = read_imagef(srcImage, sampler, coords + (int2)( 1, -1));
        colours[3] = read_imagef(srcImage, sampler, coords + (int2)(-1,  0));
        colours[4] = read_imagef(srcImage, sampler, coords + (int2)( 0,  0));
        colours[5] = read_imagef(srcImage, sampler, coords + (int2)( 1,  0));
        colours[6] = read_imagef(srcImage, sampler, coords + (int2)(-1,  1));
        colours[7] = read_imagef(srcImage, sampler, coords + (int2)( 0,  1));
        colours[8] = read_imagef(srcImage, sampler, coords + (int2)( 1,  1));

        colour  = colours[0] * kVals[0] + colours[1] * kVals[1] + colours[2] * kVals[2];
        colour += colours[3] * kVals[3] + colours[4] * kVals[4] + colours[5] * kVals[5];
        colour += colours[6] * kVals[6] + colours[7] * kVals[7] + colours[8] * kVals[8];

        colour /= normalizationVal;
        write_imagef(dstImage, coords, colour);
}
