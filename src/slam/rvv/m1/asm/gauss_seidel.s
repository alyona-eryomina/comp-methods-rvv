.text
.balign 4
.global gauss_seidel_rvv_asm

# register arguments:
#     a0      size
#     a1      A
#     a2      b
#     a3      x
#     fa0     eps
#     a4      buf
gauss_seidel_rvv_asm:
    mv a6, zero # iterations

    mv t0, a0  # size_t len = size;
    mv t2, a2  # b
    mv t3, a3  # x
    for_start_x:
        vsetvli t1, t0, e64, m1  # l = vsetvl_e64m1(len);
        sub t0, t0, t1
        slli t1, t1, 3

        vle.v v0, (t2) # vX = vle64_v_f64m1(b + offset, l);
        vse.v v0, (t3) # vse64_v_f64m1(x + offset, vX, l);

        add t2, t2, t1
        add t3, t3, t1
        bgtz t0, for_start_x
    
    for_main:
        mv t0, a0    # size_t len = size;
        mv t3, a3    # x
        ld t2, 0(a4) # tmp
        for_tmp:
            vsetvli t1, t0, e64, m1  # l = vsetvl_e64m1(len);
            sub t0, t0, t1
            slli t1, t1, 3

            vle.v v0, (t3) # vX = vle64_v_f64m1(x + offset, l);
            vse.v v0, (t2) # vse64_v_f64m1(buf->tmp_x + offset, vX, l);

            add t3, t3, t1
            add t2, t2, t1
            bgtz t0, for_tmp
        
        mv t0, zero # i
        mv t2, a1 # A
        mv t3, a3 # x
        mv a5, a2 # b
        for_x:
            slli t4, t0, 3
            add t2, t2, t4 # i * size + i
            fld ft1, (t2) # A[i * size + i]
            sub t2, t2, t4 # i * size

            fld ft2, (t3) # x[i]
            fmul.d ft2, ft2, ft1
            fsgnjn.d ft2, ft2, ft2 # -A[i * size + i] * x[i]

            #addi t1, zero, 1
            #vsetvli t4, t1, e64, m1 # len = 1
            vfmv.v.f v0, ft2 # tmp = vfmv_v_f_f64m1(-A[i * size + i] * x[i], 1);

            mv t4, a0 # len = size;
            mv t6, a3 # x
            for_ax:
                vsetvli t1, t4, e64, m1  # l = vsetvl_e64m1(len);
                sub t4, t4, t1
                slli t1, t1, 3

                vle.v v1, (t6) # vX = vle64_v_f64m1(x + offset, l);
                vle.v v2, (t2) # vA = vle64_v_f64m1(A + i * size + offset, l);
                vfmul.vv v2, v2, v1 # vX = vfmul_vv_f64m1(vA, vX, l);
                vfredosum.vs v0, v2, v0 # tmp = vfredosum_vs_f64m1_f64m1(tmp, vX, tmp, l);

                add t2, t2, t1
                add t6, t6, t1
                bgtz t4, for_ax

            #addi t1, zero, 1
            #vsetvli t4, t1, e64, m1
            vfmv.f.s ft2, v0 # vse64_v_f64m1(&var, tmp, 1);

            fld ft3, (a5) # b[i]
            fsub.d ft3, ft3, ft2 # (b[i] - var)
            fdiv.d ft3, ft3, ft1 # (b[i] - var) / A[i * size + i]
            fsd ft3, (t3) #fsw ft3, (t3) # x[i] = (b[i] - var) / A[i * size + i];
            
            addi t0, t0, 1
            addi t3, t3, 8 # x++
            addi a5, a5, 8 # b++

            blt t0, a0, for_x

        addi a6, a6, 1 # iterations++;

        # check
        #addi t1, zero, 1
        #vsetvli t0, t1, e64, m1 # len = 1  

        fcvt.d.w ft0, zero
        vfmv.v.f v0, ft0 # vNorm = vfmv_v_f_f64m1(0.0f, 1);

        mv t0, a0 # len = size;
        mv t5, a3 # x
        ld t6, 0(a4) # tmp
        for_converge:
            vsetvli t1, t0, e64, m1  # l = vsetvl_e32m1(len);
            sub t0, t0, t1
            slli t1, t1, 3

            vle.v v1, (t5) # vX = vle64_v_f64m1(x + offset, l);
            vle.v v2, (t6) # vP = vle64_v_f64m1(prev_x + offset, l);
            vfsub.vv v1, v1, v2 # vX = vfsub_vv_f64m1(vX, vP, l);
            vfmul.vv v1, v1, v1 # vX = vfmul_vv_f64m1(vX, vX, l);
            vfredosum.vs v0, v1, v0 # vNorm = vfredosum_vs_f64m1_f64m1(vNorm, vX, vNorm, l);

            add t5, t5, t1 # x++
            add t6, t6, t1 # tmp_x++
            bltz t0, for_converge

        #addi t1, zero, 1
        #vsetvli t0, t1, e64, m1
        vfmv.f.s ft0, v0 # vse64_v_f64m1(&norm, vNorm, 1);

        fsqrt.d ft0, ft0
        flt.d t3, fa0, ft0
        bnez t3, for_main

    mv a0, a6
    ret
