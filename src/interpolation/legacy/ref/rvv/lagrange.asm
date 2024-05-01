.text
.balign 4
.global lagrange_rvv_asm

# register arguments:
#     a0      src
#     a1      dst
#     a2      lenght
#     a3      pointX
#     a4      pointY
#     a5      pointLenght
lagrange_rvv_asm:
    vsetvli a6, a2, e32, m1  # l = vsetvl_e32m8(len);
    sub a2, a2, a6
    slli a6, a6, 2
    vle.v v0, (a0)           # vX = vle32_v_f32m8(src + offset, l);
    fcvt.s.w fs0, zero        
    vfmv.v.f v1, fs0         # vY = vfmv_v_f_f32m8(0.0f, l);

    addi t0, zero, 1
    fcvt.s.w fs1, t0          # 1.0f

    mv t0, zero              # uint32_t pointIndex = 0;
    for:
        bge t0, a5, done # если pointIndex >= pointLenght, завершить

        vfmv.v.f v2, fs1   # vLNumerator   = vfmv_v_f_f32m8(1.0f, l);
        vfmv.v.f v3, fs1   # vLDenominator = vfmv_v_f_f32m8(1.0f, l);
        
        slli t1, t0, 2
        add a7, a3, t1
        flw fs2, (a7)
        vfmv.v.f v4, fs2   # vPointXIndex = vfmv_v_f_f32m8(pointX[pointIndex], l);

        mv t2, zero
        for2: 
            bge t2, a5, done2 # если pointIndex >= pointLenght, завершить
            beq t0, t2, skip

            slli t1, t2, 2
            add a7, a3, t1
            flw fs2, (a7)
            vfsub.vf v5, v0, fs2
            vfsub.vf v6, v4, fs2
            vfmul.vv v2, v2, v5
            vfmul.vv v3, v3, v6

            skip:
            addi t2, t2, 1
            j for2

        done2:

        vfdiv.vv v7, v2, v3
        slli t1, t0, 2
        add a7, a4, t1
        flw fs2, (a7)
        vfmul.vf v5, v7, fs2
        vfadd.vv v1, v5, v1

        # vfadd.vv v1, v1, v2 #del

        addi t0, t0, 1 
        j for
    done:

    #vfmv.v.f v1, fs1 #del

    vse.v v1, (a1)
    add a0, a0, a6
    add a1, a1, a6

    bnez a2, lagrange_rvv_asm
    ret
