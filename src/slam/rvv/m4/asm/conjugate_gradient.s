.text
.balign 4
.global conjugate_gradient_rvv_asm

# register arguments:
#     a0      size
#     a1      A
#     a2      b
#     a3      x
#     fa0     eps
#     a4      buf: r, z, s

conjugate_gradient_rvv_asm:
    mv a6, zero # iterations

    addi t0, zero, 1
    vsetvli t1, t0, e64, m1 # len = 1
    fcvt.d.w ft0, zero
    vfmv.v.f v0, ft0 # vSpr0 = vfmv_v_f_f64m1(0.0, 1);

    # начальное приближение
    mv t0, a0 # len = size;
    mv t2, a3 # x
    ld t3, 0(a4) # r
    ld t4, 8(a4) # z
    for_start:
        vsetvli t1, t0, e64, m4  # l = vsetvl_e32m1(len);
        sub t0, t0, t1
        slli t1, t1, 3

        vfmv.v.f v4, ft0 # vX = vfmv_v_f_f64m1(0.0, l);
        vse.v v4, (t2)   # vse64_v_f64m1(x + offset, vX, l);

        vle.v v4, (a2)   # vR = vle64_v_f64m1(b + offset, l);
        vse.v v4, (t3)   # vse64_v_f64m1(buf->r + offset, vR, l);
        vse.v v4, (t4)   # vse64_v_f64m1(buf->z + offset, vR, l);

        vfmul.vv v4, v4, v4  # vR = vfmul_vv_f64m1(vR, vR, l);
        vfredosum.vs v0, v4, v0  # vSpr0 = vfredosum_vs_f64m1_f64m1(vSpr0, vR, vSpr0, l);

        add t2, t2, t1 # x++
        add a2, a2, t1 # b++
        add t3, t3, t1 # r++
        add t4, t4, t1 # z++

        bgtz t0, for_start
    
    addi t0, zero, 1
    vsetvli t1, t0, e64, m1
    vfmv.f.s ft1, v0 # vse64_v_f64m1(&spr_0, vSpr0, 1);

    for_main:
        # Вычисляем числитель и знаменатель для коэффициента
        # alpha = (rk-1,rk-1)/(Azk-1,zk-1)
        mv t0, zero # i
        mv t4, a1 # A
        ld t3, 16(a4) # s
        
        for_alpha_1:
            addi t1, zero, 1
            vsetvli t2, t1, e64, m1 # len = 1
            vfmv.v.f v0, ft0 # tmp = vfmv_v_f_f64m1(0.0, 1);

            mv a5, a0 # size
            ld a7, 8(a4) # z
            for_s:
                vsetvli t1, a5, e64, m4  # l = vsetvl_e32m1(len);
                sub a5, a5, t1
                slli t1, t1, 3

                vle.v v4, (t4) # vR = vle64_v_f64m1(A + i * size + offset, l);
                vle.v v8, (a7) # vZ = vle64_v_f64m1(buf->z + offset, l);

                vfmul.vv v4, v4, v8 # vR = vfmul_vv_f64m1(vR, vZ, l);
                vfredosum.vs v0, v4, v0  # tmp = vfredosum_vs_f64m1_f64m1(tmp, vR, tmp, l);

                add t4, t4, t1 # A++
                add a7, a7, t1 # z++
                bgtz a5, for_s
            
            addi t2, zero, 1
            vsetvli t1, t2, e64, m1
            vse.v v0, (t3) # vse64_v_f64m1(buf->s + i, tmp, 1);

            addi t3, t3, 8 # s++
            addi t0, t0, 1 # i++
            blt t0, a0, for_alpha_1
        
        vfmv.v.f v0, ft0 # vSpz = vfmv_v_f_f64m1(0.0, 1);
        mv a5, a0 # size
        ld t4, 8(a4) # z
        ld a7, 16(a4) # s
        for_alpha_2:
            vsetvli t1, a5, e64, m4  # l = vsetvl_e32m1(len);
            sub a5, a5, t1
            slli t1, t1, 3

            vle.v v4, (a7) # vS = vle64_v_f64m1(buf->s + offset, l);
            vle.v v8, (t4) # vZ = vle64_v_f64m1(buf->z + offset, l);

            vfmul.vv v4, v4, v8  # vS = vfmul_vv_f64m1(vS, vZ, l);
            vfredosum.vs v0, v4, v0 # vSpz = vfredosum_vs_f64m1_f64m1(vSpz, vS, vSpz, l);

            add t4, t4, t1 # z++
            add a7, a7, t1 # s++
            bgtz a5, for_alpha_2

        addi t2, zero, 1
        vsetvli t1, t2, e64, m1
        vfmv.f.s ft2, v0 # vse64_v_f64m1(&spz, vSpz, 1);

        fdiv.d ft3, ft1, ft2 # double alpha = spr_0 / spz;

        # Вычисляем вектор решения: xk = xk-1+ alpha * zk-1, 
        # вектор невязки: rk = rk-1 - alpha * A * zk-1 и числитель для betaa равный (rk,rk)
        vfmv.v.f v0, ft0 # Spr1 = vfmv_v_f_f64m1(0.0, 1);
        mv a5, a0 # size
        mv t4, a3 # x
        ld a7, 0(a4) # r
        ld t2, 8(a4) # z
        ld t3, 16(a4) # s
        for_beta:
            vsetvli t1, a5, e64, m4  # l = vsetvl_e32m1(len);
            sub a5, a5, t1
            slli t1, t1, 3

            vle.v v4, (t4) # vX = vle64_v_f64m1(x + offset, l);
            vle.v v8, (t2) # vZ = vle64_v_f64m1(buf->z + offset, l);
            vfmacc.vf v4, ft3, v8 # vX = vfmacc_vf_f64m1(vX, alpha, vZ, l);
            vse.v v4, (t4) # vse64_v_f64m1(x + offset, vX, l);

            vle.v v4, (a7) # vR = vle64_v_f64m1(buf->r + offset, l);
            vle.v v8, (t3) # vS = vle64_v_f64m1(buf->s + offset, l);
            vfnmsac.vf v4, ft3, v8 # vR = vfnmsac_vf_f64m1(vR, alpha, vS, l);
            vse.v v4, (a7) # vse64_v_f64m1(buf->r + offset, vR, l);

            vfmul.vv v4, v4, v4 # vR = vfmul_vv_f64m1(vR, vR, l);
            vfredosum.vs v0, v4, v0 # vSpr1 = vfredosum_vs_f64m1_f64m1(vSpr1, vR, vSpr1, l);

            add t4, t4, t1 # x++
            add t2, t2, t1 # z++
            add a7, a7, t1 # r++
            add t3, t3, t1 # s++
            bgtz a5, for_beta

        addi t2, zero, 1
        vsetvli t1, t2, e64, m1
        vfmv.f.s ft3, v0 # vse64_v_f64m1(&spr_1, vSpr1, 1);

        # вычисляем beta
        fdiv.d ft2, ft3, ft1 # beta = spr_1 / spr_0;

        # Вычисляем вектор спуска: zk = rk+ beta * zk-1
        mv a5, a0 # size
        ld a7, 0(a4) # r
        ld t2, 8(a4) # z
        for_z:
            vsetvli t1, a5, e64, m4  # l = vsetvl_e32m1(len);
            sub a5, a5, t1
            slli t1, t1, 3

            vle.v v0, (t2) # vZ = vle64_v_f64m1(buf->z + offset, l);
            vle.v v4, (a7) # vR = vle64_v_f64m1(buf->r + offset, l);
            vfmadd.vf v0, ft2, v4 # vZ = vfmadd_vf_f64m1(vZ, beta, vR, l);
            vse.v v0, (t2) # vse64_v_f64m1(buf->z + offset, vZ, l);

            add t2, t2, t1
            add a7, a7, t1
            bgtz a5, for_z
        
        fadd.d ft1, ft3, ft0 # spr_0 = spr_1;

        addi a6, a6, 1 # iterations += 1;

        fsqrt.d ft3, ft1
        flt.d  t3, fa0, ft3
        bnez t3, for_main

    mv a0, a6
    ret
