/*
 * Copyright (c) 2002, 2016 Jens Keiner, Stefan Kunis, Daniel Potts
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <complex.h>

#define NFFT_PRECISION_DOUBLE

#include "nfft3mp.h"

/* void simple_test_infft_1d(int N, int M, int iter) */
/* { */
/*   int k,l;                            /\**< index for nodes, freqencies,iter*\/ */
/*   nfft_plan p;                          /\**< plan for the nfft               *\/ */
/*   infft_plan ip;                        /\**< plan for the inverse nfft       *\/ */

/*   /\** initialise an one dimensional plan *\/ */
/*   nfft_init_1d(&p, N, M); */

/*   /\** init pseudo random nodes *\/ */
/*   nfft_vrand_shifted_unit_double(p.x,p.M_total); */

/*   /\** precompute psi, the entries of the matrix B *\/ */
/*   if(p.flags & PRE_ONE_PSI) */
/*     nfft_precompute_one_psi(&p); */

/*   /\** initialise inverse plan *\/ */
/*   infft_init(&ip,&p); */

/*   /\** init pseudo random samples and show them *\/ */
/*   nfft_vrand_unit_complex(ip.y,p.M_total); */
/*   nfft_vpr_complex(ip.y,p.M_total,"Given data, vector y"); */

/*   /\** initialise some guess f_hat_0 and solve *\/ */
/*   for(k=0;k<p.N_total;k++) */
/*       ip.f_hat_iter[k]=0; */

/*   nfft_vpr_complex(ip.f_hat_iter,p.N_total,"Initial guess, vector f_hat_iter"); */

/*   CSWAP(ip.f_hat_iter,p.f_hat); */
/*   nfft_trafo(&p); */
/*   nfft_vpr_complex(p.f,p.M_total,"Data fit, vector f"); */
/*   CSWAP(ip.f_hat_iter,p.f_hat); */

/*   infft_before_loop(&ip); */
/*   printf("\n Residual r=%e\n",ip.dot_r_iter); */

/*   for(l=0;l<iter;l++) */
/*     { */
/*       printf("\n********** Iteration l=%d **********\n",l); */
/*       infft_loop_one_step(&ip); */
/*       nfft_vpr_complex(ip.f_hat_iter,p.N_total, */
/* 		  "Approximate solution, vector f_hat_iter"); */

/*       CSWAP(ip.f_hat_iter,p.f_hat); */
/*       nfft_trafo(&p); */
/*       nfft_vpr_complex(p.f,p.M_total,"Data fit, vector f"); */
/*       CSWAP(ip.f_hat_iter,p.f_hat); */

/*       printf("\n Residual r=%e\n",ip.dot_r_iter); */
/*     } */

/*   infft_finalize(&ip); */
/*   nfft_finalize(&p); */
/* } */

/** Simple test routine for the inverse nfft */
static void simple_test_solver_nfft_1d(int N, int M, int iter)
{
  int k, l; /**< index for nodes, freqencies,iter*/
  NFFT(plan) p; /**< plan for the nfft               */
  SOLVER(plan_complex) ip; /**< plan for the inverse nfft       */
  const char *error_str;

  /** initialise an one dimensional plan */
  NFFT(init_1d)(&p, N, M);

  /** init pseudo random nodes */
  NFFT(vrand_shifted_unit_double)(p.x, p.M_total);

  /** precompute psi, the entries of the matrix B */
  if (p.flags & PRE_ONE_PSI)
    NFFT(precompute_one_psi)(&p);

  /** initialise inverse plan */
  SOLVER(init_complex)(&ip, (NFFT(mv_plan_complex)*) (&p));

  /** init pseudo random samples and show them */
  NFFT(vrand_unit_complex)(ip.y, p.M_total);
  NFFT(vpr_complex)(ip.y, p.M_total, "Given data, vector y");

  /** initialise some guess f_hat_0 and solve */
  for (k = 0; k < p.N_total; k++)
    ip.f_hat_iter[k] = NFFT_K(0.0);

  NFFT(vpr_complex)(ip.f_hat_iter, p.N_total,
      "Initial guess, vector f_hat_iter");

  /** check for valid parameters before calling any trafo/adjoint method */
  error_str = NFFT(check)(&p);
  if (error_str != 0)
  {
    printf("Error in nfft module: %s\n", error_str);
    return;
  }

  NFFT_CSWAP(ip.f_hat_iter, p.f_hat);
  NFFT(trafo)(&p);
  NFFT(vpr_complex)(p.f, p.M_total, "Data fit, vector f");
  NFFT_CSWAP(ip.f_hat_iter, p.f_hat);

  SOLVER(before_loop_complex)(&ip);
  printf("\n Residual r=%" NFFT__FES__ "\n", ip.dot_r_iter);

  for (l = 0; l < iter; l++)
  {
    printf("\n********** Iteration l=%d **********\n", l);
    SOLVER(loop_one_step_complex)(&ip);
    NFFT(vpr_complex)(ip.f_hat_iter, p.N_total,
        "Approximate solution, vector f_hat_iter");

    NFFT_CSWAP(ip.f_hat_iter, p.f_hat);
    NFFT(trafo)(&p);
    NFFT(vpr_complex)(p.f, p.M_total, "Data fit, vector f");
    NFFT_CSWAP(ip.f_hat_iter, p.f_hat);

    printf("\n Residual r=%"  NFFT__FES__ "\n", ip.dot_r_iter);
  }

  SOLVER(finalize_complex)(&ip);
  NFFT(finalize)(&p);
}

/** Main routine */
int main(void)
{
  printf("\n Computing a one dimensional inverse nfft\n");

  simple_test_solver_nfft_1d(16, 8, 9);

  return EXIT_SUCCESS;
}
/* \} */
