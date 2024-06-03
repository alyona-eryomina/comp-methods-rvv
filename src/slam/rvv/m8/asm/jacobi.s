.text
.balign 4
.global jacobi_rvv_asm

# register arguments:
#     a0      size
#     a1      A
#     a2      b
#     a3      x
#     fa0     eps
#     a4      buf

jacobi_rvv_asm:
    mv t6, zero # iterations
    mv t0, a0  # size_t len = size;
    mv t2, a2  # b
    mv t3, a3  # x
    for_start_x:
        vsetvli t1, t0, e64, m8  # l = vsetvl_e64m2(len);
        sub t0, t0, t1
        slli t1, t1, 3

        vle.v v0, (t2) # vX = vle64_v_f64m2(b + offset, l);
        vse.v v0, (t3) # vse64_v_f64m2(x + offset, vX, l);

        add t2, t2, t1
        add t3, t3, t1
        bgtz t0, for_start_x
    
    mv t0, zero # i
    ld t2, 8(a4)
    for_index:
        mul t1, t0, a0
        add t1, t1, t0
        sd t1, (t2)

        addi t2, t2, 8
        addi t0, t0, 1
        blt t0, a0, for_index
    
    for_main:
        ld t5, 0(a4)
        mv t0, zero # i
        for_ax:
            addi t3, zero, 1
            vsetvli t4, t3, e64, m1 # len = 1

            fcvt.d.w ft0, zero
            vfmv.v.f v0, ft0 # tmp = vfmv_v_f_f64m1(0.0f, 1);

            mv t3, a3 # x
            mul t4, t0, a0 # i * size
            slli t4, t4, 3
            add t4, a1, t4 # (A + i * size)
            mv t1, a0 # len = size;
            for_ax_1:
                vsetvli t2, t1, e64, m8  # l = vsetvl_e64m2(len);
                sub t1, t1, t2
                slli t2, t2, 3

                vle.v v8, (t3) # vX = vle64_v_f64m2(x + offset, l);
                vle.v v16, (t4) # vA = vle64_v_f64m2(A + i * size + offset, l);
                vfmul.vv v8, v16, v8
                vfredosum.vs v0, v8, v0

                add t3, t3, t2
                add t4, t4, t2
                bgtz t1, for_ax_1
            addi t3, zero, 1
            vsetvli t4, t3, e64, m1
            vse.v v0, (t5)

            addi t5, t5, 8
            addi t0, t0, 1
            blt t0, a0, for_ax

        mv t0, a0
        #addi t3, zero, 1
        #vsetvli t4, t3, e64, m1 # len = 1

        fcvt.d.w ft0, zero
        vfmv.v.f v0, ft0 # norm1 = vfmv_v_f_f64m1(0.0f, 1);
        mv t4, a2 # b
        mv t5, a3 # x
        ld a7, 0(a4) # buf->tmp_x
        ld t2, 8(a4) # buf->index
        for_tmp:
            vsetvli t1, t0, e64, m8  # l = vsetvl_e64m1(len);
            sub t0, t0, t1
            slli t1, t1, 3

            vle.v v16, (a7) # vAX = vle64_v_f64m1(buf->tmp_x + offset, l);

            vle.v v24, (t2) # vInd = vle64_v_u64m1(buf->index + offset, l);
            addi t3, zero, 8
            vmul.vx v24, v24, t3 # vInd = vmul_vx_u64m1(vInd, sizeof(float), l);
            vlxe.v v24, (a1), v24 # vA = vloxei64_v_f64m1(A, vInd, l);
            vle.v v8, (t5) # vX = vle64_v_f64m1(x + offset, l);

            vfmsub.vv v16, v8, v24 # vAX = vfmsac_vv_f64m1(vAX, vX, vA, l);

            vle.v v8, (t4) # vTmpX = vle64_v_f64m1(b + offset, l);
            vfadd.vv v8, v8, v16  # vTmpX = vfadd_vv_f64m1(vTmpX, vAX, l);
            vfdiv.vv v8, v8, v24  # vTmpX = vfdiv_vv_f64m1(vTmpX, vA, l);

            vle.v v16, (t5) # vX = vle64_v_f64m1(x + offset, l);
            vse.v v8, (t5) # vse64_v_f64m1(x + offset, vTmpX, l);

            vfsub.vv v16, v16, v8 # vNorm = vfsub_vv_f64m1(vX, vTmpX, l);
            vfsgnjn.vv v16, v16, v16 # vNorm = vfabs_v_f64m1(vNorm, l);
            vfredmax.vs v0, v16, v0 # norm1 = vfredmax_vs_f64m1_f64m1(norm1, vNorm, norm1, l);

            add t4, t4, t1
            add t5, t5, t1
            add t2, t2, t1
            add a7, a7, t1
            bgtz t0, for_tmp

        addi t3, zero, 1
        vsetvli t4, t3, e64, m1 
        vfmv.f.s ft1, v0 #  vse64_v_f64m1(&norm, norm1, 1);

        flt.d  t3, fa0, ft1
        addi t6, t6, 1
        bnez t3, for_main

    mv a0, t6
    ret
