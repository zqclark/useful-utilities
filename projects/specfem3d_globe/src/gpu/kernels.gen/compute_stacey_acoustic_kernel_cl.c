//note: please do not modify this file manually!
//      this file has been generated automatically by BOAST version 1.2.0
//      by: make boast_kernels

/*
!=====================================================================
!
!          S p e c f e m 3 D  G l o b e  V e r s i o n  7 . 0
!          --------------------------------------------------
!
!     Main historical authors: Dimitri Komatitsch and Jeroen Tromp
!                        Princeton University, USA
!                and CNRS / University of Marseille, France
!                 (there are currently many more authors!)
! (c) Princeton University and CNRS / University of Marseille, April 2014
!
! This program is free software; you can redistribute it and/or modify
! it under the terms of the GNU General Public License as published by
! the Free Software Foundation; either version 2 of the License, or
! (at your option) any later version.
!
! This program is distributed in the hope that it will be useful,
! but WITHOUT ANY WARRANTY; without even the implied warranty of
! MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
! GNU General Public License for more details.
!
! You should have received a copy of the GNU General Public License along
! with this program; if not, write to the Free Software Foundation, Inc.,
! 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
!
!=====================================================================
*/

const char * compute_stacey_acoustic_kernel_program = "\
inline void atomicAdd(volatile __global float *source, const float val) {\n\
  union {\n\
    unsigned int iVal;\n\
    float fVal;\n\
  } res, orig;\n\
  do {\n\
    orig.fVal = *source;\n\
    res.fVal = orig.fVal + val;\n\
  } while (atomic_cmpxchg((volatile __global unsigned int *)source, orig.iVal, res.iVal) != orig.iVal);\n\
}\n\
#ifndef INDEX2\n\
#define INDEX2(isize,i,j) i + isize*j\n\
#endif\n\
#ifndef INDEX3\n\
#define INDEX3(isize,jsize,i,j,k) i + isize*(j + jsize*k)\n\
#endif\n\
#ifndef INDEX4\n\
#define INDEX4(isize,jsize,ksize,i,j,k,x) i + isize*(j + jsize*(k + ksize*x))\n\
#endif\n\
#ifndef INDEX5\n\
#define INDEX5(isize,jsize,ksize,xsize,i,j,k,x,y) i + isize*(j + jsize*(k + ksize*(x + xsize*y)))\n\
#endif\n\
\n\
#ifndef NDIM\n\
#define NDIM 3\n\
#endif\n\
#ifndef NGLLX\n\
#define NGLLX 5\n\
#endif\n\
#ifndef NGLL2\n\
#define NGLL2 25\n\
#endif\n\
#ifndef NGLL3\n\
#define NGLL3 125\n\
#endif\n\
#ifndef NGLL3_PADDED\n\
#define NGLL3_PADDED 128\n\
#endif\n\
#ifndef N_SLS\n\
#define N_SLS 3\n\
#endif\n\
#ifndef IREGION_CRUST_MANTLE\n\
#define IREGION_CRUST_MANTLE 1\n\
#endif\n\
#ifndef IREGION_INNER_CORE\n\
#define IREGION_INNER_CORE 3\n\
#endif\n\
#ifndef IFLAG_IN_FICTITIOUS_CUBE\n\
#define IFLAG_IN_FICTITIOUS_CUBE 11\n\
#endif\n\
#ifndef R_EARTH_KM\n\
#define R_EARTH_KM 6371.0f\n\
#endif\n\
#ifndef COLORING_MIN_NSPEC_INNER_CORE\n\
#define COLORING_MIN_NSPEC_INNER_CORE 1000\n\
#endif\n\
#ifndef COLORING_MIN_NSPEC_OUTER_CORE\n\
#define COLORING_MIN_NSPEC_OUTER_CORE 1000\n\
#endif\n\
#ifndef BLOCKSIZE_TRANSFER\n\
#define BLOCKSIZE_TRANSFER 256\n\
#endif\n\
\n\
__kernel void compute_stacey_acoustic_kernel(const __global float * potential_dot_acoustic, __global float * potential_dot_dot_acoustic, const int interface_type, const int num_abs_boundary_faces, const __global int * abs_boundary_ispec, const __global int * nkmin_xi, const __global int * nkmin_eta, const __global int * njmin, const __global int * njmax, const __global int * nimin, const __global int * nimax, const __global float * abs_boundary_jacobian2D, const __global float * wgllwgll, const __global int * ibool, const __global float * vpstore, const int SAVE_FORWARD, __global float * b_absorb_potential){\n\
  int igll;\n\
  int iface;\n\
  int i;\n\
  int j;\n\
  int k;\n\
  int iglob;\n\
  int ispec;\n\
  float sn;\n\
  float jacobianw;\n\
  float fac1;\n\
  igll = get_local_id(0);\n\
  iface = get_group_id(0) + (get_group_id(1)) * (get_num_groups(0));\n\
  if (iface < num_abs_boundary_faces) {\n\
    ispec = abs_boundary_ispec[iface] - (1);\n\
    switch (interface_type) {\n\
      case 4 :\n\
        if (nkmin_xi[INDEX2(2, 0, iface)] == 0 || njmin[INDEX2(2, 0, iface)] == 0) {\n\
           return ;\n\
        }\n\
        i = 0;\n\
        k = (igll) / (NGLLX);\n\
        j = igll - ((k) * (NGLLX));\n\
        if (k < nkmin_xi[INDEX2(2, 0, iface)] - (1) || k > NGLLX - (1)) {\n\
           return ;\n\
        }\n\
        if (j < njmin[INDEX2(2, 0, iface)] - (1) || j > njmax[INDEX2(2, 0, iface)] - (1)) {\n\
           return ;\n\
        }\n\
        fac1 = wgllwgll[(k) * (NGLLX) + j];\n\
        break;\n\
      case 5 :\n\
        if (nkmin_xi[INDEX2(2, 1, iface)] == 0 || njmin[INDEX2(2, 1, iface)] == 0) {\n\
           return ;\n\
        }\n\
        i = NGLLX - (1);\n\
        k = (igll) / (NGLLX);\n\
        j = igll - ((k) * (NGLLX));\n\
        if (k < nkmin_xi[INDEX2(2, 1, iface)] - (1) || k > NGLLX - (1)) {\n\
           return ;\n\
        }\n\
        if (j < njmin[INDEX2(2, 1, iface)] - (1) || j > njmax[INDEX2(2, 1, iface)] - (1)) {\n\
           return ;\n\
        }\n\
        fac1 = wgllwgll[(k) * (NGLLX) + j];\n\
        break;\n\
      case 6 :\n\
        if (nkmin_eta[INDEX2(2, 0, iface)] == 0 || nimin[INDEX2(2, 0, iface)] == 0) {\n\
           return ;\n\
        }\n\
        j = 0;\n\
        k = (igll) / (NGLLX);\n\
        i = igll - ((k) * (NGLLX));\n\
        if (k < nkmin_eta[INDEX2(2, 0, iface)] - (1) || k > NGLLX - (1)) {\n\
           return ;\n\
        }\n\
        if (i < nimin[INDEX2(2, 0, iface)] - (1) || i > nimax[INDEX2(2, 0, iface)] - (1)) {\n\
           return ;\n\
        }\n\
        fac1 = wgllwgll[(k) * (NGLLX) + i];\n\
        break;\n\
      case 7 :\n\
        if (nkmin_eta[INDEX2(2, 1, iface)] == 0 || nimin[INDEX2(2, 1, iface)] == 0) {\n\
           return ;\n\
        }\n\
        j = NGLLX - (1);\n\
        k = (igll) / (NGLLX);\n\
        i = igll - ((k) * (NGLLX));\n\
        if (k < nkmin_eta[INDEX2(2, 1, iface)] - (1) || k > NGLLX - (1)) {\n\
           return ;\n\
        }\n\
        if (i < nimin[INDEX2(2, 1, iface)] - (1) || i > nimax[INDEX2(2, 1, iface)] - (1)) {\n\
           return ;\n\
        }\n\
        fac1 = wgllwgll[(k) * (NGLLX) + i];\n\
        break;\n\
      case 8 :\n\
        k = 0;\n\
        j = (igll) / (NGLLX);\n\
        i = igll - ((j) * (NGLLX));\n\
        if (j < 0 || j > NGLLX - (1)) {\n\
           return ;\n\
        }\n\
        if (i < 0 || i > NGLLX - (1)) {\n\
           return ;\n\
        }\n\
        fac1 = wgllwgll[(j) * (NGLLX) + i];\n\
        break;\n\
    }\n\
    iglob = ibool[INDEX4(NGLLX, NGLLX, NGLLX, i, j, k, ispec)] - (1);\n\
    sn = (potential_dot_acoustic[iglob]) / (vpstore[INDEX4(NGLLX, NGLLX, NGLLX, i, j, k, ispec)]);\n\
    jacobianw = (abs_boundary_jacobian2D[INDEX2(NGLL2, igll, iface)]) * (fac1);\n\
    atomicAdd(potential_dot_dot_acoustic + iglob, ( -(sn)) * (jacobianw));\n\
    if (SAVE_FORWARD) {\n\
      b_absorb_potential[INDEX2(NGLL2, igll, iface)] = (sn) * (jacobianw);\n\
    }\n\
  }\n\
}\n\
";