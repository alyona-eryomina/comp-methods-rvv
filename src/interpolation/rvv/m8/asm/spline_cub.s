.text
.balign 4
.global spline_cub_rvv_asm

# register arguments:
#     a0      src
#     a1      dst
#     a2      lenght
#     a3      pointX
#     a4      pointY
#     a5      pointLenght
#     a6      buf
spline_cub_rvv_asm:
    # сохраняем в стек
    addi sp, sp, -7*8
    sd s1, 48(sp)
    sd s2, 40(sp)
    sd s3, 32(sp)
    sd s4, 24(sp)
    sd s5, 16(sp)
    sd s6, 8(sp)
    sd s7, 0(sp)

    flw ft0, (a3)     # pointX[0]
    addi t0, zero, 4
    add t0, a3, t0
    flw ft1, (t0)     # pointX[1]
    fsub.s ft1, ft1, ft0 # float h = pointX[1] - pointX[0];

    addi t0, zero, 3 # 3
    fcvt.s.w ft2, t0 
    fmul.s ft3, ft1, ft1 # h*h
    fdiv.s ft2, ft2, ft3 # 3/(h*h)

    ld s1, 24(a6) # buf->va
    ld s2, 32(a6) # buf->vb
    ld s3, 40(a6) # buf->vc
    ld s4, 48(a6) # buf->F

    # записываем первые
    fcvt.s.w ft3, zero
    fsw ft3, (s1) # buf->va[0] = 0.0f;
    fsw ft3, (s3) # buf->vc[0] = 0.0f;
    fsw ft3, (s4) # buf->F[0] = 0.0f;
    addi t0, zero, 1
    fcvt.s.w ft3, t0
    fsw ft3, (s2) # buf->vb[0] = 1.0f;

    addi s1, s1, 4
    addi s2, s2, 4
    addi s3, s3, 4
    addi s4, s4, 4
    addi t0, a5, -1 # len = pointLength - 1;
    mv s5, a4 # pointY
    for_point_1:
        vsetvli t1, t0, e32, m8  # l = vsetvl_e32m8(len);

        sub t0, t0, t1
        slli t1, t1, 2

        addi t2, zero, 1
        fcvt.s.w ft3, t2 # 1
        vfmv.v.f v0, ft3 # vA = vfmv_v_f_f32m8(1.0f, l);, vC = vfmv_v_f_f32m8(1.0f, l);
        vse.v v0, (s1) # vse32_v_f32m8(buf->va + offset, vA, l);
        vse.v v0, (s3) # vse32_v_f32m8(buf->vc + offset, vC, l);

        addi t2, zero, 4
        fcvt.s.w ft3, t2 # 4
        vfmv.v.f v0, ft3 # vB = vfmv_v_f_f32m8(4.0f, l);
        vse.v v0, (s2) # vse32_v_f32m8(buf->vb + offset, vB, l);

        vle.v v0, (s5) # vPointY = vle32_v_f32m8(pointY + offset - 1, l);
        addi s5, s5, 4
        vle.v v8, (s5) # vPointY2 = vle32_v_f32m8(pointY + offset, l);
        addi s5, s5, 4

        addi t2, zero, 2
        fcvt.s.w ft3, t2 # 2
        vfmul.vf v8, v8, ft3 # vPointY2 = vfmul_vf_f32m8(vPointY2, 2.0f, l);
        vfsub.vv v0, v0, v8  # vF = vfsub_vv_f32m8(vPointY, vPointY2, l);
        
        vle.v v8, (s5) # vPointY3 = vle32_v_f32m8(pointY + offset + 1, l);
        addi s5, s5, -8

        vfadd.vv v0, v0, v8  # vF = vfadd_vv_f32m8(vF, vPointY3, l);
        vfmul.vf v0, v0, ft2 # vF = vfmul_vf_f32m8(vF, coef, l);

        vse.v v0, (s4) # vse32_v_f32m8(buf->F + offset, vF, l);

        add s1, s1, t1
        add s2, s2, t1
        add s3, s3, t1
        add s4, s4, t1
        add s5, s5, t1

        bgt t0, t2, for_point_1

    # записываем последние
    fcvt.s.w ft3, zero
    fsw ft3, -4(s1) # buf->va[pointLength - 1] = 0.0f;
    fsw ft3, -4(s3) # buf->vc[pointLength - 1] = 0.0f;
    fsw ft3, -4(s4) # buf->F[pointLength - 1] = 0.0f;
    addi t0, zero, 1
    fcvt.s.w ft3, t0
    fsw ft3, -4(s2) # buf->vb[pointLength - 1] = 1.0f;

    # прогонка
    ld s1, 24(a6) # buf->va
    ld s2, 32(a6) # buf->vb
    ld s3, 40(a6) # buf->vc
    ld s4, 48(a6) # buf->F
    ld s5, 56(a6) # buf->alpha
    ld s6, 64(a6) # buf->beta

    addi s5, s5, 4 # buf->alpha++
    addi s6, s6, 4 # buf->beta++

    flw ft7, (s2) # buf->vb[0]
    flw ft2, (s3) # buf->vc[0]
    flw ft3, (s4) # buf->vF[0]
    fdiv.s ft2, ft2, ft7 # buf->vc[0] / buf->vb[0];
    fsgnjn.s ft2, ft2, ft2 # -buf->vc[0] / buf->vb[0];
    fsw ft2, (s5) # buf->alpha[1] = - buf->vc[0] / buf->vb[0];
    fdiv.s ft3, ft3, ft7 # buf->F[0] / buf->vb[0];
    fsw ft3, (s6) # buf->beta[1] = buf->F[0] / buf->vb[0];

    addi s1, s1, 4
    addi s2, s2, 4
    addi s3, s3, 4
    addi s4, s4, 4

    addi t0, a5, -1 # len = pointLength - 1;
    addi t1, zero, 1 # 1
    for_run_1:
        addi t0, t0, -1

        flw ft4, (s5) # buf->alpha
        flw ft5, (s6) # buf->beta

        addi s5, s5, 4 # buf->alpha++
        addi s6, s6, 4 # buf->beta++

        flw ft8, (s1) # buf->va
        flw ft7, (s2) # buf->vb
        flw ft2, (s3) # buf->vc
        flw ft3, (s4) # buf->vF

        fmadd.s ft6, ft8, ft4, ft7 # (buf->va[i] * buf->alpha[i] + buf->vb[i])
        fnmsub.s ft8, ft8, ft5, ft3 # (buf->F[i] - buf->va[i] * buf->beta[i])
        fdiv.s ft2, ft2, ft6  # buf->vc[i] / (buf->va[i] * buf->alpha[i] + buf->vb[i])
        fsgnjn.s ft2, ft2, ft2  # -buf->vc[i] / (buf->va[i] * buf->alpha[i] + buf->vb[i])
        fdiv.s ft8, ft8, ft6  # (buf->F[i] - buf->va[i] * buf->beta[i]) / (buf->va[i] * buf->alpha[i] + buf->vb[i]);

        fsw ft2, (s5)
        fsw ft8, (s6)

        addi s1, s1, 4
        addi s2, s2, 4
        addi s3, s3, 4
        addi s4, s4, 4

        bgt t0, t1, for_run_1

    # buf->b[pointLength - 1] = (buf->F[pointLength - 1] - buf->va[pointLength - 1] * buf->beta[pointLength - 1]) / (buf->vb[pointLength - 1] + buf->va[pointLength - 1]  * buf->alpha[pointLength - 1]);
    addi t0, a5, -1 # len = pointLength - 1;
    
    ld s7, 8(a6) # buf->b
    slli t1, t0, 2
    add s7, s7, t1

    flw ft5, (s1) # buf->va
    flw ft7, (s2) # buf->vb
    flw ft2, (s3) # buf->vF
    flw ft3, (s4) # buf->alpha
    flw ft4, (s5) # buf->beta
    fmadd.s ft6, ft5, ft3, ft7 # (buf->va[i] * buf->alpha[i] + buf->vb[i])
    fnmsub.s ft5, ft5, ft4, ft2 # (buf->F[i] - buf->va[i] * buf->beta[i])
    fdiv.s ft5, ft5, ft6 
    fsw ft5, (s7)

    for_run_2:
        addi t0, t0, -1

        flw ft2, (s7)
        addi s7, s7, -4

        flw ft3, (s5) # buf->alpha
        flw ft4, (s6) # buf->beta

        fmadd.s ft4, ft2, ft3, ft4
        fsw ft4, (s7)

        addi s6, s6, -4
        addi s5, s5, -4

        bgt t0, zero, for_run_2

    ld s1, 0(a6) # buf->a
    ld s2, 8(a6) # buf->b
    ld s3, 16(a6) # buf->c
    mv s4, a4 # pointY
    
    addi t3, zero, 2
    fcvt.s.w ft3, t3 # 2
    addi t3, zero, 3
    fcvt.s.w ft4, t3
    fdiv.s ft5, ft1, ft4 # h/3
    fmul.s ft4, ft1, ft4 # h*3

    addi t0, a5, -1 # len = pointLength - 1;
    for_run_3:
        vsetvli t1, t0, e32, m8  # l = vsetvl_e32m8(len);
        sub t0, t0, t1
        slli t1, t1, 2

        vle.v v0, (s2) # vB = vle32_v_f32m8(buf->b + offset, l);
        addi t3, s2, 4
        vle.v v8, (t3) # vA = vle32_v_f32m8(buf->b + offset + 1, l);

        vfmul.vf v16, v0, ft3 # vC = vfmul_vf_f32m8(vB, 2, l);
        vfadd.vv v16, v16, v8 # vC = vfadd_vv_f32m8(vC, vA, l);
        vfmul.vf v16, v16, ft5  # vC = vfmul_vf_f32m8(vC, h/3, l);

        vfsub.vv v8, v8, v0 # vA = vfsub_vv_f32m8(vA, vB, l);
        vfdiv.vf v8, v8, ft4 # vA = vfdiv_vf_f32m8(vA, 3*h, l);

        vle.v v0, (s4)  # vPointY = vle32_v_f32m8(pointY + offset, l);
        addi t3, s4, 4
        vle.v v24, (t3) # vTmp = vle32_v_f32m8(pointY + offset + 1, l);
        vfsub.vv v24, v24, v0
        vfdiv.vf v24, v24, ft1
        vfsub.vv v16, v24, v16

        vse.v v8, (s1)
        vse.v v16, (s3)

        add s1, s1, t1
        add s2, s2, t1
        add s3, s3, t1
        add s4, s4, t1

        bgtz t0, for_run_3
    
    # записываем последние
    flw ft9, -4(s1)
    fsw ft9, (s1) # buf->a[pointLength - 1] = buf->a[pointLength - 2];

    flw ft5, -4(s2)
    flw ft6, -4(s3)
    fmul.s ft7, ft9, ft4 # 3 * buf->a[pointLength - 2] * h
    fmadd.s ft7, ft5, ft3, ft7 # 2 * buf->b[pointLength - 2] + 3 * buf->a[pointLength - 2] * h
    fmadd.s ft7, ft7, ft1, ft6 
    fsw ft7, (s3)

    # основные вычисления
    ld s1, 0(a6) # buf->a
    ld s2, 8(a6) # buf->b
    ld s3, 16(a6) # buf->c

    mv t0, a2  # len = length;
    for_main:
        vsetvli t1, t0, e32, m8  # l = vsetvl_e32m8(len);
        sub t0, t0, t1
        slli t1, t1, 2

        vle.v v0, (a0) # vSrc = vle32_v_f32m8(src + offset, l);
        
        vfsub.vf v8, v0, ft0 # vTmp = vfsub_vf_f32m8(vSrc, pointX[0], l);
        vfdiv.vf v8, v8, ft1 # vTmp = vfdiv_vf_f32m8(vTmp, h, l);
        vfcvt.xu.f.v v16, v8  # vNum = vfcvt_xu_f_v_u32m8(vTmp, l);
        addi t4, zero, 4
        vmul.vx v8, v16, t4   # vNum = vmul_vx_u32m8(vNum, sizeof(float), l);

        vlxe.v v16, (a3), v8  #  vPointX = vloxei32_v_f32m8(pointX, vNum, l);
        vlxe.v v24, (a4), v8  # vPointY = vloxei32_v_f32m8(pointY, vNum, l);
        vfsub.vv v16, v0, v16  # vX = vfsub_vv_f32m8(vSrc, vPointX, l);

        vlxe.v v0, (s3), v8  # vC = vloxei32_v_f32m8(buf->c, vNum, l);
        vfmadd.vv v0, v16, v24 # vTmp = vfmadd_vv_f32m8(vC, vX, vPointY, l);

        vlxe.v v24, (s1), v8  # vA = vloxei32_v_f32m8(buf->a, vNum, l);
        vlxe.v v8, (s2), v8  # vB = vloxei32_v_f32m8(buf->b, vNum, l);

        vfmadd.vv v24, v16, v8 # vDst = vfmadd_vv_f32m8(vA, vX, vB, l);
        vfmul.vv v16, v16, v16  # vX2 = vfmul_vv_f32m8(vX, vX, l);
        vfmadd.vv v24, v16, v0 # vDst = vfmadd_vv_f32m8(vDst, vX2, vTmp, l);

        vse.v v24, (a1)

        add a0, a0, t1
        add a1, a1, t1

        bgtz t0, for_main

    # востанавливаем из стека
    ld s1, 48(sp)
    ld s2, 40(sp)
    ld s3, 32(sp)
    ld s4, 24(sp)
    ld s5, 16(sp)
    ld s6, 8(sp)
    ld s7, 0(sp)
    addi sp, sp, 7*8

    ret
