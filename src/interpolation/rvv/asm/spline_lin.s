.text
.balign 4
.global spline_lin_rvv_asm

# register arguments:
#     a0      src
#     a1      dst
#     a2      lenght
#     a3      pointX
#     a4      pointY
#     a5      pointLenght
#     a6      buf
spline_lin_rvv_asm:
    flw fs0, (a3)     # pointX[0]
    addi t0, zero, 4
    add a7, a3, t0
    flw fs1, (a7)     # pointX[1]
    fsub.s fs2, fs1, fs0 # float h = pointX[1] - pointX[0];

    ld s2, 0(a6) # a
    ld s3, 8(a6) # b

    addi s1, a5, -1  # size_t len = pointLength - 1;
    #mv s1, a5
    for_point:
        vsetvli t1, s1, e32, m1  # l = vsetvl_e32m1(len);

        sub s1, s1, t1
        slli t1, t1, 2

        vle.v v0, (a4) # vPointY = vle32_v_f32m1(pointY + offset, l);
        add a7, a4, t0
        vle.v v1, (a7) # vPointY1 = vle32_v_f32m1(pointY + 1 + offset, l);
        vle.v v2, (a3) # vPointX = vle32_v_f32m1(pointX + offset, l);

        vfsub.vv v3, v1, v0  # vA = vfsub_vv_f32m1(vPointY1, vPointY, l);
        vfdiv.vf v3, v3, fs2 # vA = vfdiv_vf_f32m1(vA, h, l);
        fcvt.s.w fs1, zero
        vfmv.v.f v4, fs1
        vfadd.vv v4, v4, v3
        vfnmsub.vv v4, v2, v0 # vB = vfnmsub_vv_f32m1(vA, vPointX, vPointY, l);

        vse.v v3, (s2) # vse32_v_f32m1(buf->a + offset, vA, l);
        vse.v v4, (s3) # vse32_v_f32m1(buf->b + offset, vB, l);

        add a4, a4, t1
        add a3, a3, t1
        add s2, s2, t1
        add s3, s3, t1
        bnez s1, for_point
    # записываем последние элементы
    flw fs3, (a4)
    fsw fs3, (s2) # buf->a[pointLength - 1] = pointY[pointLength - 1];

    flw fs3, -4(s3)
    fsw fs3, (s3) # buf->b[pointLength - 1] = buf->b[pointLength - 2];

    mv s1, a2  # len = length;
    ld s2, 0(a6) # a
    ld s3, 8(a6) # b
    for_main:
        vsetvli t1, s1, e32, m1  # l = vsetvl_e32m1(len);

        sub s1, s1, t1
        slli t1, t1, 2

        vle.v v0, (a0) # vSrc = vle32_v_f32m1(src + offset, l);

        vfsub.vf v1, v0, fs0 # vTmp = vfsub_vf_f32m1(vSrc, pointX[0], l);
        vfdiv.vf v1, v1, fs2 # vTmp = vfdiv_vf_f32m1(vTmp, h, l);
        vfcvt.xu.f.v v2, v1 # vNum = vfcvt_xu_f_v_u32m1(vTmp, l);
        addi t2, zero, 4
        vmul.vx v2, v2, t2   # vNum = vmul_vx_u32m1(vNum, sizeof(float), l);

        vlxe.v v3, (s2), v2
        vlxe.v v4, (s3), v2
        vfmadd.vv v3, v0, v4

        vse.v v3, (a1)

        add a0, a0, t1
        add a1, a1, t1

        bnez s1, for_main

    ret

