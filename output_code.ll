; ModuleID = 'MainModule'
source_filename = "MainModule"

@x = global float 0.000000e+00
@y = global float 0.000000e+00
@hola = global float 0.000000e+00
@u = global float 0.000000e+00
@ret = global float 0.000000e+00

define float @run() {
entry:
  %forDebug = alloca float, align 4
  store float 2.000000e+00, ptr %forDebug, align 4
  store float 0x40179999A0000000, ptr %forDebug, align 4
  store float 4.200000e+01, ptr @x, align 4
  store float 9.000000e+00, ptr %forDebug, align 4
  store float 8.400000e+01, ptr @y, align 4
  store float -9.000000e+00, ptr @u, align 4
  call void @test_procedure()
  store float 0.000000e+00, ptr @ret, align 4
  %0 = load float, ptr @ret, align 4
  ret float %0
}

define void @test_procedure() {
entry:
  %forDebug = alloca float, align 4
  store float 4.200000e+01, ptr @hola, align 4
  store float 0x40179999A0000000, ptr %forDebug, align 4
  ret void
}
