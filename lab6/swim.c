/* swim.f -- translated by f2c (version 20100827).
   You must link the resulting object file with libf2c:
	on Microsoft Windows system, link with libf2c.lib;
	on Linux or Unix systems, link with .../path/to/libf2c.a -lm
	or, if you install libf2c.a in a standard place, with -lf2c -lm
	-- in that order, at the end of the command line, as in
		cc *.o -lf2c -lm
	Source for libf2c is in /netlib/f2c/libf2c.zip, e.g.,

		http://www.netlib.org/f2c/libf2c.zip
*/

#include "f2c.h"

/* Common Block Declarations */

struct {
    doublereal u[1782225]	/* was [1335][1335] */, v[1782225]	/* 
	    was [1335][1335] */, p[1782225]	/* was [1335][1335] */, unew[
	    1782225]	/* was [1335][1335] */, vnew[1782225]	/* was [1335][
	    1335] */, pnew[1782225]	/* was [1335][1335] */, uold[1782225]	
	    /* was [1335][1335] */, vold[1782225]	/* was [1335][1335] */
	    , pold[1782225]	/* was [1335][1335] */, cu[1782225]	/* 
	    was [1335][1335] */, cv[1782225]	/* was [1335][1335] */, z__[
	    1782225]	/* was [1335][1335] */, h__[1782225]	/* was [1335][
	    1335] */, psi[1782225]	/* was [1335][1335] */;
} _BLNK__;

#define _BLNK__1 _BLNK__

struct {
    doublereal dt, tdt, dx, dy, a, alpha;
    integer itmax, mprint, m, n, mp1, np1;
    doublereal el, pi, tpi, di, dj, pcf;
} cons_;

#define cons_1 cons_

/* Table of constant values */

static integer c__9 = 9;
static integer c__1 = 1;
static integer c__5 = 5;
static integer c__3 = 3;

/* ** Version modified for SPEC Benchmark suite - see Comments below */
/* *** */
/* Main program */ int MAIN__(void)
{
    /* Format strings */
    static char fmt_390[] = "(\002 NUMBER OF POINTS IN THE X DIRECTION\002,i"
	    "8/\002 NUMBER OF POINTS IN THE Y DIRECTION\002,i8/\002 GRID SPAC"
	    "ING IN THE X DIRECTION    \002,f8.0/\002 GRID SPACING IN THE Y D"
	    "IRECTION    \002,f8.0/\002 TIME STEP                         "
	    " \002,f8.0/\002 TIME FILTER PARAMETER              \002,f8.3/"
	    "\002 NUMBER OF ITERATIONS               \002,i8)";
    static char fmt_350[] = "(/\002 CYCLE NUMBER\002,i5,\002 MODEL TIME IN  "
	    "HOURS\002,f6.2)";
    static char fmt_360[] = "(/\002 DIAGONAL ELEMENTS OF U \002,//(8e15.7))";
    static char fmt_366[] = "(/,\002 Pcheck = \002,e12.4,/,\002 Ucheck = "
	    "\002,e12.4,/,\002 Vcheck = \002,e12.4,/)";

    /* System generated locals */
    integer i__1, i__2;
    doublereal d__1;
    olist o__1;

    /* Builtin functions */
    integer s_wsle(cilist *), do_lio(integer *, integer *, char *, ftnlen), 
	    e_wsle(void), f_open(olist *), s_wsfe(cilist *), do_fio(integer *,
	     char *, ftnlen), e_wsfe(void);
    /* Subroutine */ int s_stop(char *, ftnlen);

    /* Local variables */
    static integer i__;
    static doublereal time;
    extern /* Subroutine */ int calc1_(void), calc2_(void), calc3_(void);
    static integer mnmin;
    static doublereal ptime;
    extern /* Subroutine */ int calc3z_(void);
    static integer icheck, jcheck;
    static doublereal pcheck, ucheck, vcheck;
    static integer ncycle;
    extern /* Subroutine */ int inital_(void);

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 6, 0, 0, 0 };
    static cilist io___2 = { 0, 6, 0, 0, 0 };
    static cilist io___3 = { 0, 6, 0, fmt_390, 0 };
    static cilist io___8 = { 0, 7, 0, fmt_350, 0 };
    static cilist io___9 = { 0, 7, 0, fmt_360, 0 };
    static cilist io___16 = { 0, 6, 0, fmt_366, 0 };


/*     BENCHMARK WEATHER PREDICTION PROGRAM FOR COMPARING THE */
/*     PREFORMANCE OF CURRENT SUPERCOMPUTERS. THE MODEL IS */
/*     BASED OF THE PAPER - THE DYNAMICS OF FINITE-DIFFERENCE */
/*     MODELS OF THE SHALLOW-WATER EQUATIONS, BY ROBERT SADOURNY */
/*     J. ATM. SCIENCES, VOL 32, NO 4, APRIL 1975. */

/*     CODE BY PAUL N. SWARZTRAUBER, NATIONAL CENTER FOR */
/*     ATMOSPHERIC RESEARCH, BOULDER, CO,  OCTOBER 1984. */

/*     MODIFIED BY R. K. SATO, NCAR, APRIL 7, 1986 FOR MULTITASKING. */
/*     MODIFIED FOR SPEC to run longer: ITMAX inCremented from 120 to */
/*                                      1200 - J.Lo 7/19/90 */
/*     Modified by Bodo Parady for the SPECpar suite.  Various */
/*     compilation sizes added.  Iterations reduced to orginal */
/*     problem, but size increased 4x in each dimension. */

/*     Further modified by Reinhold Weicker (Siemens Nixdorf) for the */
/*     SPEC CFP95 suite: */
/*     Changed back to the form with PARAMETER statements for */
/*     N1 and N2, set N1 = N2 = 1335. */
/*     The execution time is determined by these values and the */
/*     variable ITMAX (number of iterations) read from the input file. */
/*     Execution time is linear in ITMAX. */
/*      PARAMETER (N1=1061, N2=1061) */


    s_wsle(&io___1);
    do_lio(&c__9, &c__1, " SPEC benchmark 171.swim", (ftnlen)24);
    e_wsle();
    s_wsle(&io___2);
    do_lio(&c__9, &c__1, " ", (ftnlen)1);
    e_wsle();
    o__1.oerr = 0;
    o__1.ounit = 7;
    o__1.ofnmlen = 5;
    o__1.ofnm = "SWIM7";
    o__1.orl = 0;
    o__1.osta = "UNKNOWN";
    o__1.oacc = 0;
    o__1.ofm = 0;
    o__1.oblnk = 0;
    f_open(&o__1);
/*       REQUEST PROCESSORS FOR MICROTASKING */
/* SPEC removed CMIC$ GETCPUS 4 */


/*       INITIALIZE CONSTANTS AND ARRAYS */

    inital_();

/*     PRINT INITIAL VALUES */

    s_wsfe(&io___3);
    do_fio(&c__1, (char *)&cons_1.n, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&cons_1.m, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&cons_1.dx, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&cons_1.dy, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&cons_1.dt, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&cons_1.alpha, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&cons_1.itmax, (ftnlen)sizeof(integer));
    e_wsfe();
    mnmin = min(cons_1.m,cons_1.n);
/* initial data writes removed for SPEC */
/*      WRITE(6,391) (POLD(I,I),I=1,MNMIN) */
/*  391 FORMAT(/' INITIAL DIAGONAL ELEMENTS OF P ', //(8E15.7)) */
/*      WRITE(6,392) (UOLD(I,I),I=1,MNMIN) */
/*  392 FORMAT(/' INITIAL DIAGONAL ELEMENTS OF U ', //(8E15.7)) */
/*      WRITE(6,393) (VOLD(I,I),I=1,MNMIN) */
/*  393 FORMAT(/' INITIAL DIAGONAL ELEMENTS OF V ', //(8E15.7)) */
/*        DETERMINE OVERHEAD OF TIMING CALLS */
/*  6/22/95 for SPEC: JWR: Initialization of TIME */
    time = 0.;
    ncycle = 0;
L90:
    ++ncycle;

/*     COMPUTE CAPITAL  U, CAPITAL V, Z AND H */

    calc1_();

/*     COMPUTE NEW VALUES U,V AND P */

    calc2_();

    time += cons_1.dt;
    if (ncycle % cons_1.mprint != 0) {
	goto L370;
    }
    ptime = time / 3600.f;

/* *** modified for SPEC results verification */
/* *** We want to ensure that all calculations were done */
/* *** so we "use" all of the computed results. */
/* *** */
/* *** Since all of the comparisons of the individual diagnal terms */
/* *** often differ in the smaller values, the check we have selected */
/* *** is to add the absolute values of all terms and print these results */

    s_wsfe(&io___8);
    do_fio(&c__1, (char *)&ncycle, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&ptime, (ftnlen)sizeof(doublereal));
    e_wsfe();
    s_wsfe(&io___9);
    i__1 = mnmin;
    for (i__ = 1; i__ <= i__1; i__ += 10) {
	do_fio(&c__1, (char *)&_BLNK__1.unew[i__ + i__ * 1335 - 1336], (
		ftnlen)sizeof(doublereal));
    }
    e_wsfe();
/* *** */
    pcheck = 0.;
    ucheck = 0.;
    vcheck = 0.;
    i__1 = mnmin;
    for (icheck = 1; icheck <= i__1; ++icheck) {
	i__2 = mnmin;
	for (jcheck = 1; jcheck <= i__2; ++jcheck) {
	    pcheck += (d__1 = _BLNK__1.pnew[icheck + jcheck * 1335 - 1336], 
		    abs(d__1));
	    ucheck += (d__1 = _BLNK__1.unew[icheck + jcheck * 1335 - 1336], 
		    abs(d__1));
	    vcheck += (d__1 = _BLNK__1.vnew[icheck + jcheck * 1335 - 1336], 
		    abs(d__1));
/* L4500: */
	}
	_BLNK__1.unew[icheck + icheck * 1335 - 1336] *= icheck % 100 / 100.f;
/* L3500: */
    }
/* *** */
/* *** */
    s_wsfe(&io___16);
    do_fio(&c__1, (char *)&pcheck, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&ucheck, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&vcheck, (ftnlen)sizeof(doublereal));
    e_wsfe();
L370:
/*        TEST FOR END OF RUN */
    if (ncycle >= cons_1.itmax) {
	s_stop("", (ftnlen)0);
    }

/*     TIME SMOOTHING AND UPDATE FOR NEXT CYCLE */

    if (ncycle <= 1) {
	calc3z_();
    } else {
	calc3_();
    }

    goto L90;
} /* MAIN__ */

/* Subroutine */ int inital_(void)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Builtin functions */
    integer s_rsle(cilist *), do_lio(integer *, integer *, char *, ftnlen), 
	    e_rsle(void);
    double atan(doublereal), sin(doublereal), cos(doublereal);

    /* Local variables */
    static integer i__, j;

    /* Fortran I/O blocks */
    static cilist io___17 = { 0, 5, 0, 0, 0 };
    static cilist io___18 = { 0, 5, 0, 0, 0 };
    static cilist io___19 = { 0, 5, 0, 0, 0 };
    static cilist io___20 = { 0, 5, 0, 0, 0 };
    static cilist io___21 = { 0, 5, 0, 0, 0 };
    static cilist io___22 = { 0, 5, 0, 0, 0 };
    static cilist io___23 = { 0, 5, 0, 0, 0 };
    static cilist io___24 = { 0, 5, 0, 0, 0 };
    static cilist io___25 = { 0, 5, 0, 0, 0 };


/*        INITIALIZE CONSTANTS AND ARRAYS */
/*           R. K. SATO 4/7/86 */



/*     NOTE BELOW THAT TWO DELTA T (TDT) IS SET TO DT ON THE FIRST */
/*     CYCLE AFTER WHICH IT IS RESET TO DT+DT. */

/* The following code  was in SWM256, however, it was replaced by */
/* 		READ statements to avoid calculations to be done during */
/* 		compile time. */

/*      DT = 20. */

/*      DX = .25E5 */
/*      DY = .25E5 */
/*      A = 1.E6 */
/*      ALPHA = .001 */
/*      ITMAX = 1200 */
/*      MPRINT = 1200 */
/*      M = N1 - 1 */
/*      N = N2 - 1 */
    s_rsle(&io___17);
    do_lio(&c__5, &c__1, (char *)&cons_1.dt, (ftnlen)sizeof(doublereal));
    e_rsle();
    s_rsle(&io___18);
    do_lio(&c__5, &c__1, (char *)&cons_1.dx, (ftnlen)sizeof(doublereal));
    e_rsle();
    s_rsle(&io___19);
    do_lio(&c__5, &c__1, (char *)&cons_1.dy, (ftnlen)sizeof(doublereal));
    e_rsle();
    s_rsle(&io___20);
    do_lio(&c__5, &c__1, (char *)&cons_1.a, (ftnlen)sizeof(doublereal));
    e_rsle();
    s_rsle(&io___21);
    do_lio(&c__5, &c__1, (char *)&cons_1.alpha, (ftnlen)sizeof(doublereal));
    e_rsle();
    s_rsle(&io___22);
    do_lio(&c__3, &c__1, (char *)&cons_1.itmax, (ftnlen)sizeof(integer));
    e_rsle();
    s_rsle(&io___23);
    do_lio(&c__3, &c__1, (char *)&cons_1.mprint, (ftnlen)sizeof(integer));
    e_rsle();
    s_rsle(&io___24);
    do_lio(&c__3, &c__1, (char *)&cons_1.m, (ftnlen)sizeof(integer));
    e_rsle();
    s_rsle(&io___25);
    do_lio(&c__3, &c__1, (char *)&cons_1.n, (ftnlen)sizeof(integer));
    e_rsle();
    cons_1.tdt = cons_1.dt;
    cons_1.mp1 = cons_1.m + 1;
    cons_1.np1 = cons_1.n + 1;
    cons_1.el = cons_1.n * cons_1.dx;
    cons_1.pi = atan(1.) * 4.;
    cons_1.tpi = cons_1.pi + cons_1.pi;
    cons_1.di = cons_1.tpi / cons_1.m;
    cons_1.dj = cons_1.tpi / cons_1.n;
    cons_1.pcf = cons_1.pi * cons_1.pi * cons_1.a * cons_1.a / (cons_1.el * 
	    cons_1.el);

/*     INITIAL VALUES OF THE STREAM FUNCTION AND P */

    i__1 = cons_1.np1;
    for (j = 1; j <= i__1; ++j) {
	i__2 = cons_1.mp1;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    _BLNK__1.psi[i__ + j * 1335 - 1336] = cons_1.a * sin((i__ - .5) * 
		    cons_1.di) * sin((j - .5) * cons_1.dj);
	    _BLNK__1.p[i__ + j * 1335 - 1336] = cons_1.pcf * (cos((i__ - 1) * 
		    2. * cons_1.di) + cos((j - 1) * 2. * cons_1.dj)) + 5e4;
/* L50: */
	}
    }

/*     INITIALIZE VELOCITIES */

    i__2 = cons_1.n;
    for (j = 1; j <= i__2; ++j) {
	i__1 = cons_1.m;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    _BLNK__1.u[i__ + 1 + j * 1335 - 1336] = -(_BLNK__1.psi[i__ + 1 + (
		    j + 1) * 1335 - 1336] - _BLNK__1.psi[i__ + 1 + j * 1335 - 
		    1336]) / cons_1.dy;
	    _BLNK__1.v[i__ + (j + 1) * 1335 - 1336] = (_BLNK__1.psi[i__ + 1 + 
		    (j + 1) * 1335 - 1336] - _BLNK__1.psi[i__ + (j + 1) * 
		    1335 - 1336]) / cons_1.dx;
/* L60: */
	}
    }

/*     PERIODIC CONTINUATION */

    i__1 = cons_1.n;
    for (j = 1; j <= i__1; ++j) {
	_BLNK__1.u[j * 1335 - 1335] = _BLNK__1.u[cons_1.m + 1 + j * 1335 - 
		1336];
	_BLNK__1.v[cons_1.m + 1 + (j + 1) * 1335 - 1336] = _BLNK__1.v[(j + 1) 
		* 1335 - 1335];
/* L70: */
    }
    i__1 = cons_1.m;
    for (i__ = 1; i__ <= i__1; ++i__) {
	_BLNK__1.u[i__ + 1 + (cons_1.n + 1) * 1335 - 1336] = _BLNK__1.u[i__];
	_BLNK__1.v[i__ - 1] = _BLNK__1.v[i__ + (cons_1.n + 1) * 1335 - 1336];
/* L75: */
    }
    _BLNK__1.u[(cons_1.n + 1) * 1335 - 1335] = _BLNK__1.u[cons_1.m];
    _BLNK__1.v[cons_1.m] = _BLNK__1.v[(cons_1.n + 1) * 1335 - 1335];
    i__1 = cons_1.np1;
    for (j = 1; j <= i__1; ++j) {
	i__2 = cons_1.mp1;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    _BLNK__1.uold[i__ + j * 1335 - 1336] = _BLNK__1.u[i__ + j * 1335 
		    - 1336];
	    _BLNK__1.vold[i__ + j * 1335 - 1336] = _BLNK__1.v[i__ + j * 1335 
		    - 1336];
	    _BLNK__1.pold[i__ + j * 1335 - 1336] = _BLNK__1.p[i__ + j * 1335 
		    - 1336];
/* L86: */
	}
    }
/*        END OF INITIALIZATION */
    return 0;
} /* inital_ */

/* SPEC removed CCMIC$ MICRO */
/* Subroutine */ int calc1_(void)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Local variables */
    static integer i__, j;
    static doublereal fsdx, fsdy;


/*        COMPUTE CAPITAL  U, CAPITAL V, Z AND H */



    fsdx = 4. / cons_1.dx;
    fsdy = 4. / cons_1.dy;
/* SPEC removed CCMIC$ DO GLOBAL */
    i__1 = cons_1.n;
    for (j = 1; j <= i__1; ++j) {
	i__2 = cons_1.m;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    _BLNK__1.cu[i__ + 1 + j * 1335 - 1336] = (_BLNK__1.p[i__ + 1 + j *
		     1335 - 1336] + _BLNK__1.p[i__ + j * 1335 - 1336]) * .5 * 
		    _BLNK__1.u[i__ + 1 + j * 1335 - 1336];
	    _BLNK__1.cv[i__ + (j + 1) * 1335 - 1336] = (_BLNK__1.p[i__ + (j + 
		    1) * 1335 - 1336] + _BLNK__1.p[i__ + j * 1335 - 1336]) * 
		    .5 * _BLNK__1.v[i__ + (j + 1) * 1335 - 1336];
	    _BLNK__1.z__[i__ + 1 + (j + 1) * 1335 - 1336] = (fsdx * (
		    _BLNK__1.v[i__ + 1 + (j + 1) * 1335 - 1336] - _BLNK__1.v[
		    i__ + (j + 1) * 1335 - 1336]) - fsdy * (_BLNK__1.u[i__ + 
		    1 + (j + 1) * 1335 - 1336] - _BLNK__1.u[i__ + 1 + j * 
		    1335 - 1336])) / (_BLNK__1.p[i__ + j * 1335 - 1336] + 
		    _BLNK__1.p[i__ + 1 + j * 1335 - 1336] + _BLNK__1.p[i__ + 
		    1 + (j + 1) * 1335 - 1336] + _BLNK__1.p[i__ + (j + 1) * 
		    1335 - 1336]);
	    _BLNK__1.h__[i__ + j * 1335 - 1336] = _BLNK__1.p[i__ + j * 1335 - 
		    1336] + (_BLNK__1.u[i__ + 1 + j * 1335 - 1336] * 
		    _BLNK__1.u[i__ + 1 + j * 1335 - 1336] + _BLNK__1.u[i__ + 
		    j * 1335 - 1336] * _BLNK__1.u[i__ + j * 1335 - 1336] + 
		    _BLNK__1.v[i__ + (j + 1) * 1335 - 1336] * _BLNK__1.v[i__ 
		    + (j + 1) * 1335 - 1336] + _BLNK__1.v[i__ + j * 1335 - 
		    1336] * _BLNK__1.v[i__ + j * 1335 - 1336]) * .25;
/* L100: */
	}
    }

/*     PERIODIC CONTINUATION */

    i__2 = cons_1.n;
    for (j = 1; j <= i__2; ++j) {
	_BLNK__1.cu[j * 1335 - 1335] = _BLNK__1.cu[cons_1.m + 1 + j * 1335 - 
		1336];
	_BLNK__1.cv[cons_1.m + 1 + (j + 1) * 1335 - 1336] = _BLNK__1.cv[(j + 
		1) * 1335 - 1335];
	_BLNK__1.z__[(j + 1) * 1335 - 1335] = _BLNK__1.z__[cons_1.m + 1 + (j 
		+ 1) * 1335 - 1336];
	_BLNK__1.h__[cons_1.m + 1 + j * 1335 - 1336] = _BLNK__1.h__[j * 1335 
		- 1335];
/* L110: */
    }
    i__2 = cons_1.m;
    for (i__ = 1; i__ <= i__2; ++i__) {
	_BLNK__1.cu[i__ + 1 + (cons_1.n + 1) * 1335 - 1336] = _BLNK__1.cu[i__]
		;
	_BLNK__1.cv[i__ - 1] = _BLNK__1.cv[i__ + (cons_1.n + 1) * 1335 - 1336]
		;
	_BLNK__1.z__[i__] = _BLNK__1.z__[i__ + 1 + (cons_1.n + 1) * 1335 - 
		1336];
	_BLNK__1.h__[i__ + (cons_1.n + 1) * 1335 - 1336] = _BLNK__1.h__[i__ - 
		1];
/* L115: */
    }
    _BLNK__1.cu[(cons_1.n + 1) * 1335 - 1335] = _BLNK__1.cu[cons_1.m];
    _BLNK__1.cv[cons_1.m] = _BLNK__1.cv[(cons_1.n + 1) * 1335 - 1335];
    _BLNK__1.z__[0] = _BLNK__1.z__[cons_1.m + 1 + (cons_1.n + 1) * 1335 - 
	    1336];
    _BLNK__1.h__[cons_1.m + 1 + (cons_1.n + 1) * 1335 - 1336] = _BLNK__1.h__[
	    0];
/*        END OF LOOP 100 CALCULATIONS */
    return 0;
} /* calc1_ */

/* SPEC removed CCMIC$ MICRO */
/* Subroutine */ int calc2_(void)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Local variables */
    static integer i__, j;
    static doublereal tdts8, tdtsdx, tdtsdy;


/*        COMPUTE NEW VALUES OF U,V,P */


    tdts8 = cons_1.tdt / 8.;
    tdtsdx = cons_1.tdt / cons_1.dx;
    tdtsdy = cons_1.tdt / cons_1.dy;
/* SPEC removed CCMIC$ DO GLOBAL */
    i__1 = cons_1.n;
    for (j = 1; j <= i__1; ++j) {
	i__2 = cons_1.m;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    _BLNK__1.unew[i__ + 1 + j * 1335 - 1336] = _BLNK__1.uold[i__ + 1 
		    + j * 1335 - 1336] + tdts8 * (_BLNK__1.z__[i__ + 1 + (j + 
		    1) * 1335 - 1336] + _BLNK__1.z__[i__ + 1 + j * 1335 - 
		    1336]) * (_BLNK__1.cv[i__ + 1 + (j + 1) * 1335 - 1336] + 
		    _BLNK__1.cv[i__ + (j + 1) * 1335 - 1336] + _BLNK__1.cv[
		    i__ + j * 1335 - 1336] + _BLNK__1.cv[i__ + 1 + j * 1335 - 
		    1336]) - tdtsdx * (_BLNK__1.h__[i__ + 1 + j * 1335 - 1336]
		     - _BLNK__1.h__[i__ + j * 1335 - 1336]);
	    _BLNK__1.vnew[i__ + (j + 1) * 1335 - 1336] = _BLNK__1.vold[i__ + (
		    j + 1) * 1335 - 1336] - tdts8 * (_BLNK__1.z__[i__ + 1 + (
		    j + 1) * 1335 - 1336] + _BLNK__1.z__[i__ + (j + 1) * 1335 
		    - 1336]) * (_BLNK__1.cu[i__ + 1 + (j + 1) * 1335 - 1336] 
		    + _BLNK__1.cu[i__ + (j + 1) * 1335 - 1336] + _BLNK__1.cu[
		    i__ + j * 1335 - 1336] + _BLNK__1.cu[i__ + 1 + j * 1335 - 
		    1336]) - tdtsdy * (_BLNK__1.h__[i__ + (j + 1) * 1335 - 
		    1336] - _BLNK__1.h__[i__ + j * 1335 - 1336]);
	    _BLNK__1.pnew[i__ + j * 1335 - 1336] = _BLNK__1.pold[i__ + j * 
		    1335 - 1336] - tdtsdx * (_BLNK__1.cu[i__ + 1 + j * 1335 - 
		    1336] - _BLNK__1.cu[i__ + j * 1335 - 1336]) - tdtsdy * (
		    _BLNK__1.cv[i__ + (j + 1) * 1335 - 1336] - _BLNK__1.cv[
		    i__ + j * 1335 - 1336]);
/* L200: */
	}
    }

/*     PERIODIC CONTINUATION */

    i__2 = cons_1.n;
    for (j = 1; j <= i__2; ++j) {
	_BLNK__1.unew[j * 1335 - 1335] = _BLNK__1.unew[cons_1.m + 1 + j * 
		1335 - 1336];
	_BLNK__1.vnew[cons_1.m + 1 + (j + 1) * 1335 - 1336] = _BLNK__1.vnew[(
		j + 1) * 1335 - 1335];
	_BLNK__1.pnew[cons_1.m + 1 + j * 1335 - 1336] = _BLNK__1.pnew[j * 
		1335 - 1335];
/* L210: */
    }
    i__2 = cons_1.m;
    for (i__ = 1; i__ <= i__2; ++i__) {
	_BLNK__1.unew[i__ + 1 + (cons_1.n + 1) * 1335 - 1336] = _BLNK__1.unew[
		i__];
	_BLNK__1.vnew[i__ - 1] = _BLNK__1.vnew[i__ + (cons_1.n + 1) * 1335 - 
		1336];
	_BLNK__1.pnew[i__ + (cons_1.n + 1) * 1335 - 1336] = _BLNK__1.pnew[i__ 
		- 1];
/* L215: */
    }
    _BLNK__1.unew[(cons_1.n + 1) * 1335 - 1335] = _BLNK__1.unew[cons_1.m];
    _BLNK__1.vnew[cons_1.m] = _BLNK__1.vnew[(cons_1.n + 1) * 1335 - 1335];
    _BLNK__1.pnew[cons_1.m + 1 + (cons_1.n + 1) * 1335 - 1336] = 
	    _BLNK__1.pnew[0];

    return 0;
} /* calc2_ */

/* Subroutine */ int calc3z_(void)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Local variables */
    static integer i__, j;


/*         TIME SMOOTHER FOR FIRST ITERATION */



    cons_1.tdt += cons_1.tdt;
    i__1 = cons_1.np1;
    for (j = 1; j <= i__1; ++j) {
	i__2 = cons_1.mp1;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    _BLNK__1.uold[i__ + j * 1335 - 1336] = _BLNK__1.u[i__ + j * 1335 
		    - 1336];
	    _BLNK__1.vold[i__ + j * 1335 - 1336] = _BLNK__1.v[i__ + j * 1335 
		    - 1336];
	    _BLNK__1.pold[i__ + j * 1335 - 1336] = _BLNK__1.p[i__ + j * 1335 
		    - 1336];
	    _BLNK__1.u[i__ + j * 1335 - 1336] = _BLNK__1.unew[i__ + j * 1335 
		    - 1336];
	    _BLNK__1.v[i__ + j * 1335 - 1336] = _BLNK__1.vnew[i__ + j * 1335 
		    - 1336];
	    _BLNK__1.p[i__ + j * 1335 - 1336] = _BLNK__1.pnew[i__ + j * 1335 
		    - 1336];
/* L400: */
	}
    }
    return 0;
} /* calc3z_ */

/* SPEC removed CCMIC$ MICRO */
/* Subroutine */ int calc3_(void)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Local variables */
    static integer i__, j;


/*         TIME SMOOTHER */



/*        TIME SMOOTHING AND UPDATE FOR NEXT CYCLE */

/* SPEC removed CCMIC$ DO GLOBAL */
    i__1 = cons_1.n;
    for (j = 1; j <= i__1; ++j) {
	i__2 = cons_1.m;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    _BLNK__1.uold[i__ + j * 1335 - 1336] = _BLNK__1.u[i__ + j * 1335 
		    - 1336] + cons_1.alpha * (_BLNK__1.unew[i__ + j * 1335 - 
		    1336] - _BLNK__1.u[i__ + j * 1335 - 1336] * 2.f + 
		    _BLNK__1.uold[i__ + j * 1335 - 1336]);
	    _BLNK__1.vold[i__ + j * 1335 - 1336] = _BLNK__1.v[i__ + j * 1335 
		    - 1336] + cons_1.alpha * (_BLNK__1.vnew[i__ + j * 1335 - 
		    1336] - _BLNK__1.v[i__ + j * 1335 - 1336] * 2.f + 
		    _BLNK__1.vold[i__ + j * 1335 - 1336]);
	    _BLNK__1.pold[i__ + j * 1335 - 1336] = _BLNK__1.p[i__ + j * 1335 
		    - 1336] + cons_1.alpha * (_BLNK__1.pnew[i__ + j * 1335 - 
		    1336] - _BLNK__1.p[i__ + j * 1335 - 1336] * 2.f + 
		    _BLNK__1.pold[i__ + j * 1335 - 1336]);
	    _BLNK__1.u[i__ + j * 1335 - 1336] = _BLNK__1.unew[i__ + j * 1335 
		    - 1336];
	    _BLNK__1.v[i__ + j * 1335 - 1336] = _BLNK__1.vnew[i__ + j * 1335 
		    - 1336];
	    _BLNK__1.p[i__ + j * 1335 - 1336] = _BLNK__1.pnew[i__ + j * 1335 
		    - 1336];
/* L300: */
	}
    }

/*     PERIODIC CONTINUATION */

    i__2 = cons_1.n;
    for (j = 1; j <= i__2; ++j) {
	_BLNK__1.uold[cons_1.m + 1 + j * 1335 - 1336] = _BLNK__1.uold[j * 
		1335 - 1335];
	_BLNK__1.vold[cons_1.m + 1 + j * 1335 - 1336] = _BLNK__1.vold[j * 
		1335 - 1335];
	_BLNK__1.pold[cons_1.m + 1 + j * 1335 - 1336] = _BLNK__1.pold[j * 
		1335 - 1335];
	_BLNK__1.u[cons_1.m + 1 + j * 1335 - 1336] = _BLNK__1.u[j * 1335 - 
		1335];
	_BLNK__1.v[cons_1.m + 1 + j * 1335 - 1336] = _BLNK__1.v[j * 1335 - 
		1335];
	_BLNK__1.p[cons_1.m + 1 + j * 1335 - 1336] = _BLNK__1.p[j * 1335 - 
		1335];
/* L320: */
    }
    i__2 = cons_1.m;
    for (i__ = 1; i__ <= i__2; ++i__) {
	_BLNK__1.uold[i__ + (cons_1.n + 1) * 1335 - 1336] = _BLNK__1.uold[i__ 
		- 1];
	_BLNK__1.vold[i__ + (cons_1.n + 1) * 1335 - 1336] = _BLNK__1.vold[i__ 
		- 1];
	_BLNK__1.pold[i__ + (cons_1.n + 1) * 1335 - 1336] = _BLNK__1.pold[i__ 
		- 1];
	_BLNK__1.u[i__ + (cons_1.n + 1) * 1335 - 1336] = _BLNK__1.u[i__ - 1];
	_BLNK__1.v[i__ + (cons_1.n + 1) * 1335 - 1336] = _BLNK__1.v[i__ - 1];
	_BLNK__1.p[i__ + (cons_1.n + 1) * 1335 - 1336] = _BLNK__1.p[i__ - 1];
/* L325: */
    }
    _BLNK__1.uold[cons_1.m + 1 + (cons_1.n + 1) * 1335 - 1336] = 
	    _BLNK__1.uold[0];
    _BLNK__1.vold[cons_1.m + 1 + (cons_1.n + 1) * 1335 - 1336] = 
	    _BLNK__1.vold[0];
    _BLNK__1.pold[cons_1.m + 1 + (cons_1.n + 1) * 1335 - 1336] = 
	    _BLNK__1.pold[0];
    _BLNK__1.u[cons_1.m + 1 + (cons_1.n + 1) * 1335 - 1336] = _BLNK__1.u[0];
    _BLNK__1.v[cons_1.m + 1 + (cons_1.n + 1) * 1335 - 1336] = _BLNK__1.v[0];
    _BLNK__1.p[cons_1.m + 1 + (cons_1.n + 1) * 1335 - 1336] = _BLNK__1.p[0];

    return 0;
} /* calc3_ */

/* Main program alias */ int shalow_ () { MAIN__ (); return 0; }
