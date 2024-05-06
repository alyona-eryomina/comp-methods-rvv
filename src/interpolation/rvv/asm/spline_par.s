.text
.balign 4
.global spline_par_rvv_asm

# register arguments:
#     a0      src
#     a1      dst
#     a2      lenght
#     a3      pointX
#     a4      pointY
#     a5      pointLenght
#     a6      buf
spline_par_rvv_asm:
    flw ft0, (a3)     # pointX[0]
    addi t0, zero, 4
    add t0, a3, t0
    flw ft1, (t0)     # pointX[1]
    fsub.s ft2, ft1, ft0 # float h = pointX[1] - pointX[0];

    # calculate b
    ld t3, 8(a6) # b

    fcvt.s.w ft3, zero 
    fcvt.s.w ft1, zero 
    fsw ft3, (t3)   # buf->b[0] = 0;
    addi t3, t3, 4 # buf->b++

    mv t4, a4 # pointY 
    flw ft4, (t4) # pointY[0]
    addi t4, t4, 4 # pointY++

    addi t0, zero, 2
    fcvt.s.w ft5, t0 # 2
    fdiv.s ft5, ft5, ft2 # 2/h
    
    addi t0, a5, -1  # size_t len = pointLength - 1;
    for_point_1:
        addi t0, t0, -1
        flw ft6, (t4) # pointY[index]
        fsub.s ft7, ft6, ft4 # (pointY[index] - pointY[index - 1])
        fmsub.s ft7, ft7, ft5, ft3 # buf->b[index] = 2 * (pointY[index] - pointY[index - 1]) / h - buf->b[index - 1];

        fsw ft7, (t3) # store b[index]

        fadd.s ft3, ft1, ft7
        fadd.s ft4, ft1, ft6

        addi t3, t3, 4 # buf->b++
        addi t4, t4, 4 # pointY++

        bnez t0, for_point_1

    # calculate a
    ld t2, 0(a6) # a
    ld t3, 8(a6) # b

    addi t0, zero, 2
    fcvt.s.w ft5, t0 # 2
    fmul.s ft5, ft5, ft2 # 2*h

    addi t0, a5, -1  # size_t len = pointLength - 1;
    for_point_2:
        vsetvli t1, t0, e32, m1  # l = vsetvl_e32m1(len);

        sub t0, t0, t1
        slli t1, t1, 2

        vle.v v0, (t3) # vB = vle32_v_f32m1(buf->b + offset, l);
        addi t4, t3, 4
        vle.v v1, (t4) # vA = vle32_v_f32m1(buf->b + offset + 1, l);

        vfsub.vv v1, v1, v0 # vA = vfsub_vv_f32m1(vA, vB, l);
        vfdiv.vf v1, v1, ft5 # vA = vfdiv_vf_f32m1(vA, 2*h, l);

        vse.v v1, (t2) # vse32_v_f32m1(buf->a + offset, vA, l);

        add t2, t2, t1
        add t3, t3, t1
        bnez t0, for_point_2
    
    # записываем последние
    flw ft3, -4(t2)
    fsw ft3, (t2) # buf->a[pointLength - 1] = buf->a[pointLength - 2];

    flw ft4, -4(t3)
    fmadd.s ft4, ft3, ft5, ft4
    fsw ft4, (t3) # buf->b[pointLength - 1] = buf->b[pointLength - 2] + 2 * buf->a[pointLength - 2] * h;
    
    # основное вычисление
    ld t2, 0(a6) # a
    ld t3, 8(a6) # b

    mv t0, a2 
    for_main:
        vsetvli t1, t0, e32, m1  # l = vsetvl_e32m1(len);
        sub t0, t0, t1
        slli t1, t1, 2

        vle.v v0, (a0) # vSrc = vle32_v_f32m1(src + offset, l);

        vfsub.vf v2, v0, ft0 # vTmp = vfsub_vf_f32m1(vSrc, pointX[0], l);
        vfdiv.vf v2, v2, ft2 # vTmp = vfdiv_vf_f32m1(vTmp, h, l);
        vfcvt.xu.f.v v3, v2 # vNum = vfcvt_xu_f_v_u32m1(vTmp, l);
        addi t4, zero, 4
        vmul.vx v3, v3, t4   # vNum = vmul_vx_u32m1(vNum, sizeof(float), l);

        vlxe.v v2, (t2), v3 # vA = vloxei32_v_f32m1(buf->a, vNum, l);
        vlxe.v v4, (t3), v3 # vB = vloxei32_v_f32m1(buf->b, vNum, l);
        vlxe.v v5, (a4), v3 # vC = vloxei32_v_f32m1(pointY, vNum, l);
        vlxe.v v6, (a3), v3 # vX = vloxei32_v_f32m1(pointX, vNum, l);

        vfsub.vv v6, v0, v6  # vSrc2 = vfsub_vv_f32m1(vSrc, vX, l);
        vfmadd.vv v4, v6, v5 # vDst = vfmadd_vv_f32m1(vB, vSrc2, vC, l);
        vfmul.vv v6, v6, v6  # vSrc2 = vfmul_vv_f32m1(vSrc2, vSrc2, l);
        vfmadd.vv v2, v6, v4 # vDst = vfmadd_vv_f32m1(vA, vSrc2, vDst, l);

        vse.v v2, (a1)

        add a0, a0, t1
        add a1, a1, t1

        bnez t0, for_main
    ret
