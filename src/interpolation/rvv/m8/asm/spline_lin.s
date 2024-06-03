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
    flw ft0, (a3)     # pointX[0]
    addi t0, zero, 4
    add t0, a3, t0
    flw ft1, (t0)     # pointX[1]
    fsub.s ft1, ft1, ft0 # float h = pointX[1] - pointX[0];

    ld t2, 0(a6) # a
    ld t3, 8(a6) # b

    addi t0, a5, -1  # size_t len = pointLength - 1;
    for_point:
        vsetvli t1, t0, e32, m8  # l = vsetvl_e32m8(len);

        sub t0, t0, t1
        slli t1, t1, 2

        vle.v v0, (a4) # vPointY = vle32_v_f32m8(pointY + offset, l);
        addi t4, a4, 4
        vle.v v8, (t4) # vPointY1 = vle32_v_f32m8(pointY + 1 + offset, l);

        vfsub.vv v8, v8, v0  # vA = vfsub_vv_f32m8(vPointY1, vPointY, l);
        vfdiv.vf v8, v8, ft1 # vA = vfdiv_vf_f32m8(vA, h, l);
        vse.v v8, (t2) # vse32_v_f32m8(buf->a + offset, vA, l);

        vle.v v16, (a3) # vPointX = vle32_v_f32m8(pointX + offset, l);
        vfnmsub.vv v8, v16, v0 # vB = vfnmsub_vv_f32m8(vA, vPointX, vPointY, l);
        vse.v v8, (t3) # vse32_v_f32m8(buf->b + offset, vB, l);

        add a4, a4, t1
        add a3, a3, t1
        add t2, t2, t1
        add t3, t3, t1
        bnez t0, for_point
    # записываем последние элементы
    flw ft2, (a4)
    fsw ft2, (t2) # buf->a[pointLength - 1] = pointY[pointLength - 1];

    flw ft2, -4(t3)
    fsw ft2, (t3) # buf->b[pointLength - 1] = buf->b[pointLength - 2];

    mv t0, a2  # len = length;
    ld t2, 0(a6) # a
    ld t3, 8(a6) # b
    for_main:
        vsetvli t1, t0, e32, m8  # l = vsetvl_e32m8(len);

        sub t0, t0, t1
        slli t1, t1, 2

        vle.v v0, (a0) # vSrc = vle32_v_f32m8(src + offset, l);

        vfsub.vf v8, v0, ft0 # vTmp = vfsub_vf_f32m8(vSrc, pointX[0], l);
        vfdiv.vf v8, v8, ft1 # vTmp = vfdiv_vf_f32m8(vTmp, h, l);
        vfcvt.xu.f.v v16, v8 # vNum = vfcvt_xu_f_v_u32m8(vTmp, l);
        addi t4, zero, 4
        vmul.vx v16, v16, t4   # vNum = vmul_vx_u32m8(vNum, sizeof(float), l);

        vlxe.v v24, (t2), v16
        vlxe.v v8, (t3), v16
        vfmadd.vv v24, v0, v8

        vse.v v24, (a1)

        add a0, a0, t1
        add a1, a1, t1

        bnez t0, for_main

    ret

