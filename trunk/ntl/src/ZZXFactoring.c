
#include <NTL/ZZXFactoring.h>
#include <NTL/lzz_pX.h>
#include <NTL/lzz_pXFactoring.h>
#include <NTL/ZZ_pX.h>
#include <NTL/vec_long.h>
#include <NTL/vec_vec_long.h>
#include <NTL/vec_ZZ.h>
#include <NTL/vec_double.h>
#include <math.h>


struct LocalInfoT {
   long n;
   long NumPrimes;
   long NumFactors;
   vec_long p;
   vec_vec_long pattern;
   ZZ PossibleDegrees;
   PrimeSeq s;
};



static
void mul(ZZ_pX& x, vec_ZZ_pX& a)
// this performs multiplications in close-to-optimal order,
// and kills a in the process
{
   long n = a.length();

   // first, deal with some trivial cases

   if (n == 0) {
      set(x);
      a.kill();
      return;
   }
   else if (n == 1) {
      x = a[0];
      a.kill();
      return;
   }

   long i, j;

   // assume n > 1 and all a[i]'s are nonzero

   // sort into non-increasing degrees

   for (i = 1; i <= n - 1; i++)
      for (j = 0; j <= n - i - 1; j++)
         if (deg(a[j]) < deg(a[j+1]))
            swap(a[j], a[j+1]);

   ZZ_pX g;

   while (n > 1) {
      // replace smallest two poly's by their product
      mul(g, a[n-2], a[n-1]);
      a[n-2].kill();
      a[n-1].kill();
      swap(g, a[n-2]);
      n--;

      // re-establish order

      i = n-1;
      while (i > 0 && deg(a[i-1]) < deg(a[i])) {
         swap(a[i-1], a[i]);
         i--;
      }
   }

   x = a[0];

   a[0].kill();
   a.SetLength(0);
}


void mul(ZZX& x, const vec_pair_ZZX_long& a)
{
   long l = a.length();
   ZZX res;
   long i, j;

   set(res);
   for (i = 0; i < l; i++)
      for (j = 0; j < a[i].b; j++)
         mul(res, res, a[i].a);

   x = res;
}


void SquareFreeDecomp(vec_pair_ZZX_long& u, const ZZX& ff)
// input is primitive 
{
   ZZX f = ff;

   ZZX d, v, w, s, t1;
   long i;

   u.SetLength(0);

   if (deg(f) <= 0)
      return;

   diff(t1, f);
   GCD(d, f, t1);

   if (deg(d) == 0) {
      append(u, cons(f, 1));
      return;
   }

   divide(v, f, d); 
   divide(w, t1, d);
   i = 0;

   for (;;) {
      i = i + 1;

      diff(t1, v);
      sub(s, w, t1);

      if (IsZero(s)) {
         if (deg(v) != 0) append(u, cons(v, i));
         return;
      }

      GCD(d, v, s);
      divide(v, v, d);
      divide(w, s, d);

      if (deg(d) != 0) append(u, cons(d, i));
   }
}




static
void HenselLift(ZZX& Gout, ZZX& Hout, ZZX& Aout, ZZX& Bout,
                const ZZX& f, const ZZX& g, const ZZX& h,
                const ZZX& a, const ZZX& b, const ZZ& p) 
{
   ZZX c, g1, h1, G, H, A, B;

   mul(c, g, h);
   sub(c, f, c);

   if (!divide(c, c, p))
      Error("inexact division");

   ZZ_pX cc, gg, hh, aa, bb, tt, gg1, hh1;

   conv(cc, c);
   conv(gg, g);
   conv(hh, h);
   conv(aa, a);
   conv(bb, b);

   ZZ_pXModulus GG;
   ZZ_pXModulus HH;

   build(GG, gg);
   build(HH, hh);

   ZZ_pXMultiplier AA;
   ZZ_pXMultiplier BB;

   build(AA, aa, HH);
   build(BB, bb, GG);

   rem(gg1, cc, GG);
   MulMod(gg1, gg1, BB, GG);
   
   rem(hh1, cc, HH);
   MulMod(hh1, hh1, AA, HH);

   conv(g1, gg1);
   mul(g1, g1, p);
   add(G, g, g1);

   conv(h1, hh1);
   mul(h1, h1, p);
   add(H, h, h1);

   /* lift inverses */

   ZZX t1, t2, r;

   mul(t1, a, G);
   mul(t2, b, H);
   add(t1, t1, t2);
   add(t1, t1, -1);
   negate(t1, t1);

   if (!divide(r, t1, p))
      Error("inexact division");

   ZZ_pX rr, aa1, bb1;

   conv(rr, r);
   
   rem(aa1, rr, HH);
   MulMod(aa1, aa1, AA, HH);
   rem(bb1, rr, GG);
   MulMod(bb1, bb1, BB, GG);

   ZZX a1, b1;

   conv(a1, aa1);
   mul(a1, a1, p);
   add(A, a, a1);

   conv(b1, bb1);
   mul(b1, b1, p);
   add(B, b, b1);

   Gout = G;
   Hout = H;
   Aout = A;
   Bout = B;
}

static
void HenselLift1(ZZX& Gout, ZZX& Hout, 
                const ZZX& f, const ZZX& g, const ZZX& h,
                const ZZX& a, const ZZX& b, const ZZ& p) 
{
   ZZX c, g1, h1, G, H;

   mul(c, g, h);
   sub(c, f, c);

   if (!divide(c, c, p))
      Error("inexact division");

   ZZ_pX cc, gg, hh, aa, bb, tt, gg1, hh1;

   conv(cc, c);
   conv(gg, g);
   conv(hh, h);
   conv(aa, a);
   conv(bb, b);

   ZZ_pXModulus GG;
   ZZ_pXModulus HH;

   build(GG, gg);
   build(HH, hh);

   rem(gg1, cc, GG);
   MulMod(gg1, gg1, bb, GG);
   
   rem(hh1, cc, HH);
   MulMod(hh1, hh1, aa, HH);

   conv(g1, gg1);
   mul(g1, g1, p);
   add(G, g, g1);

   conv(h1, hh1);
   mul(h1, h1, p);
   add(H, h, h1);

   Gout = G;
   Hout = H;
}

static
void BuildTree(vec_long& link, vec_ZZX& v, vec_ZZX& w,
               const vec_zz_pX& a)
{
   long k = a.length();

   if (k < 2) Error("bad arguments to BuildTree");

   vec_zz_pX V, W;

   V.SetLength(2*k-2);
   W.SetLength(2*k-2);
   link.SetLength(2*k-2);

   long i, j, s;
   long minp, mind;

   for (i = 0; i < k; i++) {
      V[i] = a[i];
      link[i] = -(i+1);
   }

   for (j = 0; j < 2*k-4; j += 2) {
      minp = j;
      mind = deg(V[j]);

      for (s = j+1; s < i; s++)
         if (deg(V[s]) < mind) {
            minp = s;
            mind = deg(V[s]);
         }

      swap(V[j], V[minp]);
      swap(link[j], link[minp]);

      minp = j+1;
      mind = deg(V[j+1]);

      for (s = j+2; s < i; s++)
         if (deg(V[s]) < mind) {
            minp = s;
            mind = deg(V[s]);
         }

      swap(V[j+1], V[minp]);
      swap(link[j+1], link[minp]);

      mul(V[i], V[j], V[j+1]);
      link[i] = j;
      i++;
   }

   zz_pX d;

   for (j = 0; j < 2*k-2; j += 2) {
      XGCD(d, W[j], W[j+1], V[j], V[j+1]);
      if (!IsOne(d))
         Error("relatively prime polynomials expected");
   }

   v.SetLength(2*k-2);
   for (j = 0; j < 2*k-2; j++)
      conv(v[j], V[j]);

   w.SetLength(2*k-2);
   for (j = 0; j < 2*k-2; j++)
      conv(w[j], W[j]);
}

static
void RecTreeLift(const vec_long& link, vec_ZZX& v, vec_ZZX& w,
                 const ZZ& p, const ZZX& f, long j, long inv)
{
   if (j < 0) return;

   if (inv)
      HenselLift(v[j], v[j+1], w[j], w[j+1],
                 f, v[j], v[j+1], w[j], w[j+1], p);
   else
      HenselLift1(v[j], v[j+1], f, v[j], v[j+1], w[j], w[j+1], p);

   RecTreeLift(link, v, w, p, v[j], link[j], inv);
   RecTreeLift(link, v, w, p, v[j+1], link[j+1], inv);
}

static
void TreeLift(const vec_long& link, vec_ZZX& v, vec_ZZX& w, 
              long e0, long e1, const ZZX& f, long inv)

// lift from p^{e0} to p^{e1}

{
   ZZ p0, p1;

   power(p0, zz_p::modulus(), e0);
   power(p1, zz_p::modulus(), e1-e0);

   ZZ_pBak bak;
   bak.save();
   ZZ_p::init(p1);

   RecTreeLift(link, v, w, p0, f, v.length()-2, inv);

   bak.restore();
} 

void MultiLift(vec_ZZX& A, const vec_zz_pX& a, const ZZX& f, long e,
               long verbose)

{
   long k = a.length();
   long i;

   if (k < 2 || e < 1) Error("MultiLift: bad args");

   if (!IsOne(LeadCoeff(f)))
      Error("MultiLift: bad args");

   for (i = 0; i < a.length(); i++)
      if (!IsOne(LeadCoeff(a[i])))
         Error("MultiLift: bad args");

   if (e == 1) {
      A.SetLength(k);
      for (i = 0; i < k; i++)
         conv(A[i], a[i]);
      return;
   }

   vec_long E;
   append(E, e);
   while (e > 1) {
      e = (e+1)/2;
      append(E, e);
   }
   long l = E.length();

   vec_ZZX v, w;
   vec_long link;

   double t;

   if (verbose) {
      cerr << "building tree...";
      t = GetTime();
   }

   BuildTree(link, v, w, a);

   if (verbose) cerr << (GetTime()-t) << "\n";


   for (i = l-1; i > 0; i--) {
      if (verbose) {
         cerr << "lifting to " << E[i-1] << "...";
         t = GetTime();
      }
      
      TreeLift(link, v, w, E[i], E[i-1], f, i != 1);

      if (verbose) cerr << (GetTime()-t) << "\n";
   }

   A.SetLength(k);
   for (i = 0; i < 2*k-2; i++) {
      long t = link[i];
      if (t < 0)
         A[-(t+1)] = v[i];
   }
}

static
void inplace_rev(ZZX& f)
{
   long n = deg(f);
   long i, j;

   i = 0;
   j = n;
   while (i < j) {
      swap(f.rep[i], f.rep[j]);
      i++;
      j--;
   }

   f.normalize();
}

long ZZXFac_InitNumPrimes = 7;
long ZZXFac_MaxNumPrimes = 50;

static 
void RecordPattern(vec_long& pat, vec_pair_zz_pX_long& fac)
{
   long n = pat.length()-1;
   long i;

   for (i = 0; i <= n; i++)
      pat[i] = 0;

   long k = fac.length();

   for (i = 0; i < k; i++) {
      long d = fac[i].b;
      long m = deg(fac[i].a)/d;

      pat[d] = m;
   }
}

static
long NumFactors(const vec_long& pat)
{
   long n = pat.length()-1;

   long i;
   long res = 0;

   for (i = 0; i <= n; i++) 
      res += pat[i];

   return res;
}

static
void CalcPossibleDegrees(ZZ& pd, const vec_long& pat)
{
   long n = pat.length()-1;
   set(pd);

   long d, j;
   ZZ t1;

   for (d = 1; d <= n; d++) 
      for (j = 0; j < pat[d]; j++) {
         LeftShift(t1, pd, d);
         bit_or(pd, pd, t1);
      }
}

static 
void CalcPossibleDegrees(vec_ZZ& S, const vec_ZZ_pX& fac, long k)

// S[i] = possible degrees of the product of any subset of size k
//        among fac[i...], encoded as a bit vector.      

{
   long r = fac.length();

   S.SetLength(r);

   if (r == 0)
      return;

   if (k < 1 || k > r)
      Error("CalcPossibleDegrees: bad args");

   long i, l;
   ZZ old, t1;

   set(S[r-1]);
   LeftShift(S[r-1], S[r-1], deg(fac[r-1]));

   for (i = r-2; i >= 0; i--) {
      set(t1);
      LeftShift(t1, t1, deg(fac[i]));
      bit_or(S[i], t1, S[i+1]);
   }

   for (l = 2; l <= k; l++) {
      old = S[r-l];
      LeftShift(S[r-l], S[r-l+1], deg(fac[r-l]));

      for (i = r-l-1; i >= 0; i--) {
         LeftShift(t1, old, deg(fac[i]));
         old = S[i];
         bit_or(S[i], S[i+1], t1);
      }
   }
}



static
vec_zz_pX *
SmallPrimeFactorization(LocalInfoT& LocalInfo, const ZZX& f,
                            long verbose)

{
   long n = deg(f);
   long i;
   double t;

   LocalInfo.n = n;
   long& NumPrimes = LocalInfo.NumPrimes;
   NumPrimes = 0;

   LocalInfo.NumFactors = 0;

   // some sanity checking...

   if (ZZXFac_InitNumPrimes < 1 || ZZXFac_InitNumPrimes > 10000)
      Error("bad ZZXFac_InitNumPrimes");

   if (ZZXFac_MaxNumPrimes < ZZXFac_InitNumPrimes || ZZXFac_MaxNumPrimes > 10000)
      Error("bad ZZXFac_MaxNumPrimes");

   LocalInfo.p.SetLength(ZZXFac_InitNumPrimes);
   LocalInfo.pattern.SetLength(ZZXFac_InitNumPrimes);
  
   // set bits 0..n of LocalInfo.PossibleDegrees 
   SetBit(LocalInfo.PossibleDegrees, n+1);
   add(LocalInfo.PossibleDegrees, LocalInfo.PossibleDegrees, -1);

   long minr = n+1;
   long irred = 0;

   vec_pair_zz_pX_long *bestfac = 0;
   zz_pX *besth = 0;
   vec_zz_pX *spfactors = 0;
   zz_pContext bestp;
   long bestp_index;

   long maxroot = NextPowerOfTwo(deg(f))+1;

   for (; NumPrimes < ZZXFac_InitNumPrimes;) {
      long p = LocalInfo.s.next();
      if (!p) Error("out of small primes");
      if (divide(LeadCoeff(f), p)) {
         if (verbose) cerr << "skipping " << p << "\n";
         continue;
      }
      zz_p::init(p, maxroot);

      zz_pX ff, ffp, d;

      conv(ff, f);
      MakeMonic(ff);
      diff(ffp, ff);

      GCD(d, ffp, ff);
      if (!IsOne(d)) {
         if (verbose)  cerr << "skipping " << p << "\n";
         continue;
      }


      if (verbose) {
         cerr << "factoring mod " << p << "...";
         t = GetTime();
      }

      vec_pair_zz_pX_long thisfac;
      zz_pX thish; 

      SFCanZass1(thisfac, thish, ff, 0);

      LocalInfo.p[NumPrimes] = p;

      vec_long& pattern = LocalInfo.pattern[NumPrimes];
      pattern.SetLength(n+1);

      RecordPattern(pattern, thisfac);
      long r = NumFactors(pattern);
      
      if (verbose) {
         cerr << (GetTime()-t) << "\n";
         cerr << "degree sequence: ";
         for (i = 0; i <= n; i++)
            if (pattern[i]) {
               cerr << pattern[i] << "*" << i << " ";
            }
         cerr << "\n";
      }

      if (r == 1) {
         irred = 1;
         break;
      }

      // update admissibility info

      ZZ pd;

      CalcPossibleDegrees(pd, pattern);
      bit_and(LocalInfo.PossibleDegrees, LocalInfo.PossibleDegrees, pd);

      if (weight(LocalInfo.PossibleDegrees) == 2) {
         irred = 1;
         break;
      }


      if (r < minr) {
         minr = r;
         delete bestfac;
         bestfac = new vec_pair_zz_pX_long;
         *bestfac = thisfac;
         delete besth;
         besth = new zz_pX;
         *besth = thish;
         bestp.save();
         bestp_index = NumPrimes;
      }

      NumPrimes++;
   }

   if (!irred) {
      // delete best prime from LocalInfo
      swap(LocalInfo.pattern[bestp_index], LocalInfo.pattern[NumPrimes-1]);
      LocalInfo.p[bestp_index] = LocalInfo.p[NumPrimes-1];
      NumPrimes--;

      bestp.restore();

      spfactors = new vec_zz_pX;

      if (verbose) {
         cerr << "p = " << zz_p::modulus() << ", completing factorization...";
         t = GetTime();
      }
      SFCanZass2(*spfactors, *bestfac, *besth, 0);
      if (verbose) {
         cerr << (GetTime()-t) << "\n";
      }
   }

   delete bestfac;
   delete besth;

   return spfactors;
}


static
long ConstTermTest(const vec_ZZ_pX& W, 
                  const vec_long& I,
                  const ZZ& ct,
                  const ZZ_p& lc,
                  vec_ZZ_p& prod,
                  long& ProdLen) 
{
   long k = I.length();
   ZZ_p t;
   ZZ t1, t2;
   long i;

   if (ProdLen == 0) {
      mul(prod[0], lc, ConstTerm(W[I[0]]));
      ProdLen++;
   }

   for (i = ProdLen; i < k; i++)
      mul(prod[i], prod[i-1], ConstTerm(W[I[i]]));

   ProdLen = k-1;

   // should make this a routine in ZZ_p
   t1 = rep(prod[k-1]);
   RightShift(t2, ZZ_p::modulus(), 1);
   if (t1 > t2)
      sub(t1, t1, ZZ_p::modulus());

   return divide(ct, t1);
}

static
void BalCopy(ZZX& g, const ZZ_pX& G)
{
   const ZZ& p = ZZ_p::modulus();
   ZZ p2, t;
   RightShift(p2, p, 1);

   long n = G.rep.length();
   long i;

   g.rep.SetLength(n);
   for (i = 0; i < n; i++) {
      t = rep(G.rep[i]);
      if (t > p2) sub(t, t, p);
      g.rep[i] = t;
   }
}




static
void mul(ZZ_pX& g, const vec_ZZ_pX& W, const vec_long& I)
{
   vec_ZZ_pX w;
   long k = I.length();
   w.SetLength(k);
   long i;

   for (i = 0; i < k; i++)
      w[i] = W[I[i]];

   mul(g, w);
}




static
void InvMul(ZZ_pX& g, const vec_ZZ_pX& W, const vec_long& I)
{
   vec_ZZ_pX w;
   long k = I.length();
   long r = W.length();
   w.SetLength(r-k);
   long i, j;

   i = 0;
   for (j = 0; j < r; j++) {
      if (i < k && j == I[i])
         i++;
      else
         w[j-i] = W[j];
   } 

   mul(g, w);
}




static
void RemoveFactors(vec_ZZ_pX& W, const vec_long& I)
{
   long k = I.length();
   long r = W.length();
   long i, j;

   i = 0;
   for (j = 0; j < r; j++) {
      if (i < k && j == I[i])
         i++;
      else
         swap(W[j-i], W[j]); 
   }

   W.SetLength(r-k);
}

static
void unpack(vec_long& x, const ZZ& a, long n)
{
   x.SetLength(n+1);
   long i;

   for (i = 0; i <= n; i++)
      x[i] = bit(a, i);
}

static
void SubPattern(vec_long& p1, const vec_long& p2)
{
   long l = p1.length();

   if (p2.length() != l)
      Error("SubPattern: bad args");

   long i;

   for (i = 0; i < l; i++) {
      p1[i] -= p2[i];
      if (p1[i] < 0)
         Error("SubPattern: internal error");
   }
}

static
void UpdateLocalInfo(LocalInfoT& LocalInfo, vec_ZZ& pdeg,
                     const vec_ZZ_pX& W, const vec_ZZX& factors,
                     const ZZX& f, long k, long verbose)
{
   _BUFFER long cnt = 0;

   if (verbose) {
      cnt = (cnt + 1) % 100;
      if (!cnt) cerr << "#";
   }

   double t;
   long i, j;

   if (LocalInfo.NumFactors < factors.length()) {
      zz_pBak bak;
      bak.save();

      vec_long pattern;
      pattern.SetLength(LocalInfo.n+1);

      ZZ pd;

      if (verbose) {
         cerr << "updating local info...";
         t = GetTime();
      }

      for (i = 0; i < LocalInfo.NumPrimes; i++) {
         zz_p::init(LocalInfo.p[i], NextPowerOfTwo(LocalInfo.n)+1);

         for (j = LocalInfo.NumFactors; j < factors.length(); j++) {
            vec_pair_zz_pX_long thisfac;
            zz_pX thish; 

            zz_pX ff;
            conv(ff, factors[j]);
            MakeMonic(ff);

            SFCanZass1(thisfac, thish, ff, 0);
            RecordPattern(pattern, thisfac);
            SubPattern(LocalInfo.pattern[i], pattern);
         }

         CalcPossibleDegrees(pd, LocalInfo.pattern[i]);
         bit_and(LocalInfo.PossibleDegrees, LocalInfo.PossibleDegrees, pd);

      }

      bak.restore();
      LocalInfo.NumFactors = factors.length();

      CalcPossibleDegrees(pdeg, W, k);

      if (verbose) cerr << (GetTime()-t) << "\n";
   }

   if (LocalInfo.NumPrimes < ZZXFac_MaxNumPrimes) {
      if (verbose)
         cerr << "adding a prime\n";

      zz_pBak bak;
      bak.save();

      for (;;) {
         long p = LocalInfo.s.next();
         if (!p)
            Error("UpdateLocalInfo: out of primes");

         if (divide(LeadCoeff(f), p)) {
            if (verbose) cerr << "skipping " << p << "\n";
            continue;
         }

         zz_p::init(p, NextPowerOfTwo(deg(f))+1);

         zz_pX ff, ffp, d;
   
         conv(ff, f);
         MakeMonic(ff);
         diff(ffp, ff);
   
         GCD(d, ffp, ff);
         if (!IsOne(d)) {
            if (verbose)  cerr << "skipping " << p << "\n";
            continue;
         }

         vec_pair_zz_pX_long thisfac;
         zz_pX thish;

         if (verbose) {
            cerr << "factoring mod " << p << "...";
            t = GetTime();
         }

         SFCanZass1(thisfac, thish, ff, 0);

         LocalInfo.p.SetLength(LocalInfo.NumPrimes+1);
         LocalInfo.pattern.SetLength(LocalInfo.NumPrimes+1);

         LocalInfo.p[LocalInfo.NumPrimes] = p;
         vec_long& pattern = LocalInfo.pattern[LocalInfo.NumPrimes];

         pattern.SetLength(LocalInfo.n+1);
         RecordPattern(pattern, thisfac);

         if (verbose) {
            cerr << (GetTime()-t) << "\n";
            cerr << "degree sequence: ";
            for (i = 0; i <= LocalInfo.n; i++)
               if (pattern[i]) {
                  cerr << pattern[i] << "*" << i << " ";
               }
            cerr << "\n";
         }

         ZZ pd;
         CalcPossibleDegrees(pd, pattern);
         bit_and(LocalInfo.PossibleDegrees, LocalInfo.PossibleDegrees, pd);

         LocalInfo.NumPrimes++;

         break;
      }

      bak.restore();
   }
}



const int ZZX_OVERLIFT = NTL_BITS_PER_LONG;
  // number of bits by which we "overlift"....this enables, in particular,
  // the "n-1" test.  Must be no larger than NTL_BITS_PER_LONG.


#define EXTRA_BITS (1)
// Any small number, like 1, 2 or 3, should be OK.


static
void CardinalitySearch(vec_ZZX& factors, ZZX& f, 
                       vec_ZZ_pX& W, 
                       LocalInfoT& LocalInfo, 
                       long k,
                       long bnd,
                       long verbose)
{
   double start_time, end_time;

   if (verbose) {
      start_time = GetTime();
      cerr << "\n************ ";
      cerr << "start cardinality " << k << "\n";
   }

   vec_long I, D;
   I.SetLength(k);
   D.SetLength(k);

   long r = W.length();

   vec_ZZ_p prod;
   prod.SetLength(k);
   long ProdLen;

   vec_ZZ pdeg;
   CalcPossibleDegrees(pdeg, W, k);

   ZZ pd;
   vec_long upd;

   long i, state;

   long cnt = 0;

   ZZ ct;
   mul(ct, ConstTerm(f), LeadCoeff(f));

   ZZ_p lc;
   conv(lc, LeadCoeff(f));

   ZZ_pX gg;
   ZZX g, h;

   I[0] = 0;  

   while (I[0] <= r-k) {
      bit_and(pd, pdeg[I[0]], LocalInfo.PossibleDegrees);

      if (IsZero(pd)) {
         if (verbose) cerr << "skipping\n";
         goto done;
      }

      unpack(upd, pd, LocalInfo.n);

      D[0] = deg(W[I[0]]);
      i = 1;
      state = 0;
      ProdLen = 0;

      for (;;) {
         if (i < ProdLen)
            ProdLen = i;

         if (i == k) {
            // process indices I[0], ..., I[k-1]

            if (cnt > 2000000) { 
               cnt = 0;
               UpdateLocalInfo(LocalInfo, pdeg, W, factors, f, k, verbose);
               bit_and(pd, pdeg[I[0]], LocalInfo.PossibleDegrees);
               if (IsZero(pd)) {
                  if (verbose) cerr << "skipping\n";
                  goto done;
               }
               unpack(upd, pd, LocalInfo.n);
            }

            state = 1;  // default continuation state


            if (!upd[D[k-1]]) {
               i--;
               cnt++;
               continue;
            }

            if (!ConstTermTest(W, I, ct, lc, prod, ProdLen)) {
               i--;
               cnt += 100;
               continue;
            }

            if (verbose) {
               cerr << "+";
            }

            cnt += 1000;

            if (2*D[k-1] <= deg(f)) {
               mul(gg, W, I);
               mul(gg, gg, lc);
               BalCopy(g, gg);
               if(MaxBits(g) > bnd) {
                  i--;
                  continue;
               }
               if (verbose) {
                  cerr << "*";
               }
               PrimitivePart(g, g);
               if (!divide(h, f, g)) {
                  i--;
                  continue;
               }
               
               // factor found!
               append(factors, g);
               if (verbose) {
                 cerr << "degree " << deg(g) << " factor found\n";
               }
               f = h;
               mul(ct, ConstTerm(f), LeadCoeff(f));
               conv(lc, LeadCoeff(f));
            }
            else {
               InvMul(gg, W, I);
               mul(gg, gg, lc);
               BalCopy(g, gg);
               if(MaxBits(g) > bnd) {
                  i--;
                  continue;
               }
               if (verbose) {
                  cerr << "*";
               }
               PrimitivePart(g, g);
               if (!divide(h, f, g)) {
                  i--;
                  continue;
               }

               // factor found!
               append(factors, h);
               if (verbose) {
                 cerr << "degree " << deg(h) << " factor found\n";
               }
               f = g;
               mul(ct, ConstTerm(f), LeadCoeff(f));
               conv(lc, LeadCoeff(f));
            }

            RemoveFactors(W, I);
            r = W.length();
            cnt = 0;

            if (2*k > r) 
               goto done;
            else 
               break;
         }
         else if (state == 0) {
            I[i] = I[i-1] + 1;
            D[i] = D[i-1] + deg(W[I[i]]);
            i++;
         }
         else { // state == 1
            I[i]++;
            if (i == 0) break;

            if (I[i] > r-k+i)
               i--;
            else {
               D[i] = D[i-1] + deg(W[I[i]]);
               i++;
               state = 0;
            }
         }
      }
   }


   done: 


   if (verbose) {
      end_time = GetTime();
      cerr << "\n************ ";
      cerr << "end cardinality " << k << "\n";
      cerr << "time: " << (end_time-start_time) << "\n";
   }
}



typedef long TBL_T;

#if (NTL_BITS_PER_LONG >= 64)

// for 64-bit machines

#define TBL_MSK (63)
#define TBL_SHAMT (6)

#else

// for 32-bit machines

#define TBL_MSK (31)
#define TBL_SHAMT (5)

#endif


#if 0

// recursive version

static
void RecInitTab(TBL_T ***lookup_tab, long i, const vec_long& ratio, 
             long r, long k, long thresh1, long **shamt_tab,
             long sum, long card, long j)
{
   if (j >= i || card >= k-1) {
      if (card > 1) {
         long shamt = shamt_tab[i][card];
         unsigned long index1 = (((unsigned long) (-sum)) >> shamt);
         lookup_tab[i][card][index1 >> TBL_SHAMT] |= (1L << (index1 & TBL_MSK));
         unsigned long index2 = (((unsigned long) (-sum+thresh1)) >> shamt);
         if (index1 != index2)
            lookup_tab[i][card][index2 >> TBL_SHAMT] |= (1L << (index2 & TBL_MSK));

      }

      return;
   }


   RecInitTab(lookup_tab, i, ratio, r, k, thresh1, shamt_tab, sum, card, j+1);
   RecInitTab(lookup_tab, i, ratio, r, k, thresh1, shamt_tab, 
              sum+ratio[r-1-j], card+1, j+1);
}


static
void DoInitTab(TBL_T ***lookup_tab, long i, const vec_long& ratio, 
               long r, long k, long thresh1, long **shamt_tab)
{
   RecInitTab(lookup_tab, i, ratio, r, k, thresh1, shamt_tab, 0, 0, 0);
}

#else

// iterative version


static
void DoInitTab(TBL_T ***lookup_tab, long i, const vec_long& ratio,
               long r, long k, long thresh1, long **shamt_tab)
{
   vec_long sum_vec, card_vec, location_vec;
   sum_vec.SetLength(i+1);
   card_vec.SetLength(i+1);
   location_vec.SetLength(i+1);

   long j = 0;
   sum_vec[0] = 0;
   card_vec[0] = 0;

   long  sum, card, location;

   location = 0;

   while (j >= 0) {
      sum = sum_vec[j];
      card = card_vec[j];

      switch (location) {

      case 0:

         if (j >= i || card >= k-1) {
            if (card > 1) {
               long shamt = shamt_tab[i][card];
               unsigned long index1 = (((unsigned long) (-sum)) >> shamt);
               lookup_tab[i][card][index1 >> TBL_SHAMT] |= (1L << (index1 & TBL_MSK));
               unsigned long index2 = (((unsigned long) (-sum+thresh1)) >> shamt);
               if (index1 != index2)
                  lookup_tab[i][card][index2 >> TBL_SHAMT] |= (1L << (index2 & TBL_MSK));
      
            }
      
            location = location_vec[j];
            j--;
            continue;
         }


         sum_vec[j+1] = sum;
         card_vec[j+1] = card;
         location_vec[j+1] = 1;
         j++;
         location = 0;
         continue;

      case 1:

         sum_vec[j+1] = sum+ratio[r-1-j];
         card_vec[j+1] = card+1;
         location_vec[j+1] = 2;
         j++;
         location = 0;
         continue;  

      case 2:

         location = location_vec[j];
         j--;
         continue;
      }
   }
}
         
#endif
   
   

static
void InitTab(TBL_T ***lookup_tab, const vec_long& ratio, long r, long k,
             long thresh1, long **shamt_tab, long pruning)
{
   long i, j, t;

   if (pruning) {
      for (i = 2; i <= pruning; i++) {
         long len = min(k-1, i);
         for (j = 2; j <= len; j++) {
            long ub = (((1L << (NTL_BITS_PER_LONG-shamt_tab[i][j])) 
                      + TBL_MSK) >> TBL_SHAMT); 
            for (t = 0; t < ub; t++)
               lookup_tab[i][j][t] = 0;
         }
   
         DoInitTab(lookup_tab, i, ratio, r, k, thresh1, shamt_tab);
      }
   }
}


static
void RatioInit1(vec_long& ratio, const vec_ZZ_pX& W, const ZZ_p& lc,
                long pruning, TBL_T ***lookup_tab, 
                vec_vec_long& pair_ratio, long k, long thresh1, 
                long **shamt_tab)
{
   long r = W.length();
   long i, j;

   ZZ_p a;

   ZZ p;
   p = ZZ_p::modulus();

   ZZ aa;

   for (i = 0; i < r; i++) {
      long m = deg(W[i]);
      mul(a, W[i].rep[m-1], lc);
      LeftShift(aa, rep(a), NTL_BITS_PER_LONG);
      div(aa, aa, p);
      ratio[i] = to_long(aa);
   }

   InitTab(lookup_tab, ratio, r, k, thresh1, shamt_tab, pruning);

   for (i = 0; i < r; i++)
      for (j = 0; j < i; j++) {
         mul(a, W[i].rep[deg(W[i])-1], W[j].rep[deg(W[j])-1]);
         mul(a, a, lc);
         LeftShift(aa, rep(a), NTL_BITS_PER_LONG);
         div(aa, aa, p);
         pair_ratio[i][j] = to_long(aa);
      }

   for (i = 0; i < r; i++) {
      long m = deg(W[i]);
      if (m >= 2) {
         mul(a, W[i].rep[m-2], lc);
         LeftShift(aa, rep(a), NTL_BITS_PER_LONG);
         div(aa, aa, p);
         pair_ratio[i][i] = to_long(aa);
      }
      else
         pair_ratio[i][i] = 0;
   }
}

static 
long SecondOrderTest(const vec_long& I_vec, const vec_vec_long& pair_ratio_vec,
                     vec_long& sum_stack_vec, long& SumLen)
{
   long k = I_vec.length();
   const long *I = I_vec.elts();
   long *sum_stack = sum_stack_vec.elts();

   long sum, thresh1;

   if (SumLen == 0) {
      long epsilon = (1L << (NTL_BITS_PER_LONG-ZZX_OVERLIFT));
      long delta = (k*(k+1)) >> 1;
      long thresh = epsilon + delta;
      thresh1 = (epsilon << 1) + delta;

      sum = thresh;
      sum_stack[k] = thresh1;
   }
   else {
      sum = sum_stack[SumLen-1];
      thresh1 = sum_stack[k];
   }

   long i, j;

   for (i = SumLen; i < k; i++) {
      const long *p = pair_ratio_vec[I[i]].elts();
      for (j = 0; j <= i; j++) {
         sum += p[I[j]];
      }

      sum_stack[i] = sum;
   }

   SumLen = k-1;

   return (((unsigned long) sum) <= ((unsigned long) thresh1));
}


static
ZZ choose_fn(long r, long k)
{
   ZZ a, b;

   a = 1; 
   b = 1;

   long i;
   for (i = 0; i < k; i++) {
      a *= r-i;
      b *= k-i;
   }

   return a/b;
}

static
void PrintInfo(const char *s, const ZZ& a, const ZZ& b)
{
   cerr << s << a << " / " << b << " = ";
   
   double x = to_double(a)/to_double(b);

   if (x == 0) 
      cerr << "0"; 
   else {
      int n;
      double f;

      f = frexp(x, &n);
      cerr << f << "*2^" << n;
   }

   cerr << "\n";
}

static
void RemoveFactors1(vec_long& W, const vec_long& I, long r)
{
   long k = I.length();
   long i, j;

   i = 0;
   for (j = 0; j < r; j++) {
      if (i < k && j == I[i])
         i++;
      else
         swap(W[j-i], W[j]); 
   }
}

static
void RemoveFactors1(vec_vec_long& W, const vec_long& I, long r)
{
   long k = I.length();
   long i, j;

   i = 0;
   for (j = 0; j < r; j++) {
      if (i < k && j == I[i])
         i++;
      else
         swap(W[j-i], W[j]); 
   }

   for (i = 0; i < r-k; i++)
      RemoveFactors1(W[i], I, r);
}

static
void RemoveFactors1(vec_ZZ_p& W, const vec_long& I, long r)
{
   long k = I.length();
   long i, j;

   i = 0;
   for (j = 0; j < r; j++) {
      if (i < k && j == I[i])
         i++;
      else
         swap(W[j-i], W[j]);
   }
}

static
void SumCoeffs(ZZ& sum, const ZZX& a)
{
   ZZ res;
   res = 0;
   long i;
   long n = a.rep.length();
   for (i = 0; i < n; i++)
      res += a.rep[i];

   sum = res;
}

static
void SumCoeffs(ZZ_p& sum, const ZZ_pX& a)
{
   ZZ_p res;
   res = 0;
   long i;
   long n = a.rep.length();
   for (i = 0; i < n; i++)
      res += a.rep[i];

   sum = res;
}


static
long ConstTermTest(const vec_ZZ_p& W, 
                  const vec_long& I,
                  const ZZ& ct,
                  const ZZ_p& lc,
                  vec_ZZ_p& prod,
                  long& ProdLen) 
{
   long k = I.length();
   ZZ_p t;
   ZZ t1, t2;
   long i;

   if (ProdLen == 0) {
      mul(prod[0], lc, W[I[0]]);
      ProdLen++;
   }

   for (i = ProdLen; i < k; i++)
      mul(prod[i], prod[i-1], W[I[i]]);

   ProdLen = k-1;

   // should make this a routine in ZZ_p
   t1 = rep(prod[k-1]);
   RightShift(t2, ZZ_p::modulus(), 1);
   if (t1 > t2)
      sub(t1, t1, ZZ_p::modulus());

   return divide(ct, t1);
}


long ZZXFac_MaxPrune = 10;



// The following routine should only be called for k > 1,
// and is only worth calling for k > 2.

static
long pruning_bnd(long r, long k)
{
   double x = 0; 

   long i;
   for (i = 0; i < k; i++) {
      x += log(double(r-i)/double(k-i));
   }

   return long((x/log(2.0)) * 0.75);
}

static
long shamt_tab_init(long pos, long card, long pruning, long thresh1_len)
{
   double x = 1;
   long i;

   for (i = 0; i < card; i++) {
      x *= double(pos-i)/double(card-i);
   }

   x *= pruning;  // this can be adjusted to control the density
   if (pos <= 6) x *= 2;  // a little boost that costs very little
      

   long t = long(ceil(log(x)/log(2.0)));

   t = max(t, TBL_SHAMT); 

   t = min(t, NTL_BITS_PER_LONG-thresh1_len);


   return NTL_BITS_PER_LONG-t;
}


static
void CardinalitySearch1(vec_ZZX& factors, ZZX& f, 
                       vec_ZZ_pX& W, 
                       LocalInfoT& LocalInfo, 
                       long k,
                       long bnd,
                       long verbose)
{
   double start_time, end_time;

   if (verbose) {
      start_time = GetTime();
      cerr << "\n************ ";
      cerr << "start cardinality " << k << "\n";
   }

   if (k <= 1) Error("internal error: call CardinalitySearch");

   // This test is needed to ensure correcntes of "n-2" test
   if (NumBits(k) > NTL_BITS_PER_LONG/2-2)
      Error("Cardinality Search: k too large...");

   vec_ZZ pdeg;
   CalcPossibleDegrees(pdeg, W, k);
   ZZ pd;

   bit_and(pd, pdeg[0], LocalInfo.PossibleDegrees);
   if (pd == 0) {
      if (verbose) cerr << "skipping\n";
      return;
   }

   vec_long I, D;
   I.SetLength(k);
   D.SetLength(k);

   long r = W.length();

   long initial_r = r;

   vec_long ratio, ratio_sum;
   ratio.SetLength(r);
   ratio_sum.SetLength(k);

   long epsilon = (1L << (NTL_BITS_PER_LONG-ZZX_OVERLIFT));
   long delta = k;
   long thresh = epsilon + delta;
   long thresh1 = 2*epsilon + delta;

   long thresh1_len = NumBits(thresh1); 

   long pruning;

   pruning = min(r/2, ZZXFac_MaxPrune);
   pruning = min(pruning, pruning_bnd(r, k));
   pruning = min(pruning, NTL_BITS_PER_LONG-EXTRA_BITS-thresh1_len);

   if (pruning <= 4) pruning = 0;

   long init_pruning = pruning;

   TBL_T ***lookup_tab = 0;

   long **shamt_tab = 0;

   if (pruning) {
      typedef long *long_p;

      long i, j;

      shamt_tab = new long_p[pruning+1];
      if (!shamt_tab) Error("out of mem");
      shamt_tab[0] = shamt_tab[1] = 0;

      for (i = 2; i <= pruning; i++) {
         long len = min(k-1, i);
         shamt_tab[i] = new long[len+1];
         if (!shamt_tab[i]) Error("out of mem");
         shamt_tab[i][0] = shamt_tab[i][1] = 0;

         for (j = 2; j <= len; j++)
            shamt_tab[i][j] = shamt_tab_init(i, j, pruning, thresh1_len);
      }

      typedef  TBL_T *TBL_T_p;
      typedef  TBL_T **TBL_T_pp;

      lookup_tab = new TBL_T_pp[pruning+1];
      if (!lookup_tab) Error("out of mem");

      lookup_tab[0] = lookup_tab[1] = 0;

      for (i = 2; i <= pruning; i++) {
         long len = min(k-1, i);
         lookup_tab[i] = new TBL_T_p[len+1];
         if (!lookup_tab[i]) Error("out of mem");

         lookup_tab[i][0] = lookup_tab[i][1] = 0;

         for (j = 2; j <= len; j++) {
            lookup_tab[i][j] = new TBL_T[((1L << (NTL_BITS_PER_LONG-shamt_tab[i][j]))+TBL_MSK) >> TBL_SHAMT];
            if (!lookup_tab[i][j]) Error("out of mem");
         }
      }
   }

   if (verbose) {
      cerr << "pruning = " << pruning << "\n";
   }

   vec_ZZ_p prod;
   prod.SetLength(k);
   long ProdLen;

   vec_ZZ_p prod1;
   prod1.SetLength(k);
   long ProdLen1;

   vec_long sum_stack;
   sum_stack.SetLength(k+1);
   long SumLen;

   vec_long upd;

   long i, state;

   long cnt = 0;

   ZZ ct;
   mul(ct, ConstTerm(f), LeadCoeff(f));

   ZZ_p lc;
   conv(lc, LeadCoeff(f));

   vec_vec_long pair_ratio;
   pair_ratio.SetLength(r);
   for (i = 0; i < r; i++)
      pair_ratio[i].SetLength(r);

   RatioInit1(ratio, W, lc, pruning, lookup_tab, pair_ratio, k, thresh1, shamt_tab);

   ZZ c1;
   SumCoeffs(c1, f);
   mul(c1, c1, LeadCoeff(f));

   vec_ZZ_p sum_coeffs;
   sum_coeffs.SetLength(r);
   for (i = 0; i < r; i++)
      SumCoeffs(sum_coeffs[i], W[i]);

   vec_long degv;
   degv.SetLength(r);

   for (i = 0; i < r; i++)
      degv[i] = deg(W[i]);

   ZZ_pX gg;
   ZZX g, h;

   I[0] = 0;  

   long loop_cnt = 0, degree_cnt = 0, n2_cnt = 0, sl_cnt = 0, ct_cnt = 0, 
        pl_cnt = 0, c1_cnt = 0, pl1_cnt = 0, td_cnt = 0;

   ZZ loop_total, degree_total, n2_total, sl_total, ct_total, 
      pl_total, c1_total, pl1_total, td_total;

   while (I[0] <= r-k) {
      bit_and(pd, pdeg[I[0]], LocalInfo.PossibleDegrees);

      if (IsZero(pd)) {
         if (verbose) cerr << "skipping\n";
         goto done;
      }

      unpack(upd, pd, LocalInfo.n);

      D[0] = degv[I[0]];
      ratio_sum[0] = ratio[I[0]] + thresh;
      i = 1;
      state = 0;
      ProdLen = 0;
      ProdLen1 = 0;
      SumLen = 0;

      for (;;) {
         cnt++;

         if (cnt > 2000000) { 
            if (verbose) {
               loop_total += loop_cnt;  loop_cnt = 0;
               degree_total += degree_cnt;  degree_cnt = 0;
               n2_total += n2_cnt;  n2_cnt = 0;
               sl_total += sl_cnt;  sl_cnt = 0;
               ct_total += ct_cnt;  ct_cnt = 0;
               pl_total += pl_cnt;  pl_cnt = 0;
               c1_total += c1_cnt;  c1_cnt = 0;
               pl1_total += pl1_cnt;  pl1_cnt = 0;
               td_total += td_cnt;  td_cnt = 0;
            }

            cnt = 0;
            UpdateLocalInfo(LocalInfo, pdeg, W, factors, f, k, verbose);
            bit_and(pd, pdeg[I[0]], LocalInfo.PossibleDegrees);
            if (IsZero(pd)) {
               if (verbose) cerr << "skipping\n";
               goto done;
            }
            unpack(upd, pd, LocalInfo.n);
         }

         if (i == k-1) {

            long ratio_sum_last = ratio_sum[k-2];
            long I_last = I[k-2];


            {
               long D_last = D[k-2];
   
               long rs;
               long I_this;
               long D_this;
   
               for (I_this = I_last+1; I_this < r; I_this++) {
                  loop_cnt++;
   
                  rs = ratio_sum_last + ratio[I_this];
                  if (((unsigned long) rs) > ((unsigned long) thresh1)) {
                     cnt++;
                     continue;
                  }

                  degree_cnt++;
   
                  D_this = D_last + degv[I_this];
   
                  if (!upd[D_this]) {
                     cnt++;
                     continue;
                  }
   
                  n2_cnt++;
                  sl_cnt += (k-SumLen);

                  I[k-1] = I_this;

                  if (!SecondOrderTest(I, pair_ratio, sum_stack, SumLen)) {
                     cnt += 2;
                     continue;
                  }

                  c1_cnt++;
                  pl1_cnt += (k-ProdLen1);

                  if (!ConstTermTest(sum_coeffs, I, c1, lc, prod1, ProdLen1)) {
                     cnt += 100;
                     continue;
                  }

                  ct_cnt++;
                  pl_cnt += (k-ProdLen);

                  D[k-1] = D_this;

                  if (!ConstTermTest(W, I, ct, lc, prod, ProdLen)) {
                     cnt += 100;
                     continue;
                  }

                  td_cnt++;
   
                  if (verbose) {
                     cerr << "+";
                  }
   
                  cnt += 1000;
   
                  if (2*D[k-1] <= deg(f)) {
                     mul(gg, W, I);
                     mul(gg, gg, lc);
                     BalCopy(g, gg);
                     if(MaxBits(g) > bnd) {
                        continue;
                     }
                     if (verbose) {
                        cerr << "*";
                     }
                     PrimitivePart(g, g);
                     if (!divide(h, f, g)) {
                        continue;
                     }
                  
                     // factor found!
                     append(factors, g);
                     if (verbose) {
                       cerr << "degree " << deg(g) << " factor found\n";
                     }
                     f = h;
                     mul(ct, ConstTerm(f), LeadCoeff(f));
                     conv(lc, LeadCoeff(f));
                  }
                  else {
                     InvMul(gg, W, I);
                     mul(gg, gg, lc);
                     BalCopy(g, gg);
                     if(MaxBits(g) > bnd) {
                        continue;
                     }
                     if (verbose) {
                        cerr << "*";
                     }
                     PrimitivePart(g, g);
                     if (!divide(h, f, g)) {
                        continue;
                     }
      
                     // factor found!
                     append(factors, h);
                     if (verbose) {
                       cerr << "degree " << deg(h) << " factor found\n";
                     }
                     f = g;
                     mul(ct, ConstTerm(f), LeadCoeff(f));
                     conv(lc, LeadCoeff(f));
                  }
      
                  RemoveFactors(W, I);
                  RemoveFactors1(degv, I, r);
                  RemoveFactors1(sum_coeffs, I, r);
                  RemoveFactors1(ratio, I, r);
                  RemoveFactors1(pair_ratio, I, r);

                  r = W.length();
                  cnt = 0;

                  pruning = min(pruning, r/2);
                  if (pruning <= 4) pruning = 0;

                  InitTab(lookup_tab, ratio, r, k, thresh1, shamt_tab, pruning);

                  if (2*k > r) 
                     goto done;
                  else 
                     goto restart;
               } /* end of inner for loop */ 

            }

            i--;
            state = 1;  
         }
         else {
            if (state == 0) {
               long I_i = I[i-1] + 1;
               I[i] = I_i;

               long pruned;

               if (pruning && r-I_i <= pruning) {
                  long pos = r-I_i;
                  long rs = ratio_sum[i-1];
                  unsigned long index1 = (((unsigned long) (rs)) >> shamt_tab[pos][k-i]);
                  if (lookup_tab[pos][k-i][index1 >> TBL_SHAMT] & (1L << (index1&TBL_MSK)))
                     pruned = 0;
                  else
                     pruned = 1;
               }
               else
                  pruned = 0; 

               if (pruned) {
                  i--;
                  state = 1;
               }
               else {
                  D[i] = D[i-1] + degv[I_i];
                  ratio_sum[i] = ratio_sum[i-1] + ratio[I_i];
                  i++;
               }
            }
            else { // state == 1
      
               loop_cnt++;
      
               if (i < ProdLen)
                  ProdLen = i;
      
               if (i < ProdLen1)
                  ProdLen1 = i;
      
               if (i < SumLen)
                  SumLen = i;

               long I_i = (++I[i]);

               if (i == 0) break;
   
               if (I_i > r-k+i) {
                  i--;
               }
               else {

                  long pruned;

                  if (pruning && r-I_i <= pruning) {
                     long pos = r-I_i;
                     long rs = ratio_sum[i-1];
                     unsigned long index1 = (((unsigned long) (rs)) >> shamt_tab[pos][k-i]);
                     if (lookup_tab[pos][k-i][index1 >> TBL_SHAMT] & (1L << (index1&TBL_MSK)))
                        pruned = 0;
                     else
                        pruned = 1;
                  }
                  else
                     pruned = 0; 
   

                  if (pruned) {
                     i--;
                  }
                  else {
                     D[i] = D[i-1] + degv[I_i];
                     ratio_sum[i] = ratio_sum[i-1] + ratio[I_i];
                     i++;
                     state = 0;
                  }
               }
            }
         }
      }

      restart: ;
   }

   done:

   if (lookup_tab) {
      long i, j;
      for (i = 2; i <= init_pruning; i++) {
         long len = min(k-1, i);
         for (j = 2; j <= len; j++) {
            delete [] lookup_tab[i][j];
         }

         delete [] lookup_tab[i];
      }

      delete [] lookup_tab;
   }

   if (shamt_tab) {
      long i;
      for (i = 2; i <= init_pruning; i++) {
         delete [] shamt_tab[i];
      }

      delete [] shamt_tab;
   }

   if (verbose) { 
      end_time = GetTime();
      cerr << "\n************ ";
      cerr << "end cardinality " << k << "\n";
      cerr << "time: " << (end_time-start_time) << "\n";
      ZZ loops_max = choose_fn(initial_r+1, k);
      ZZ tuples_max = choose_fn(initial_r, k);

      loop_total += loop_cnt;
      degree_total += degree_cnt;
      n2_total += n2_cnt;
      sl_total += sl_cnt;
      ct_total += ct_cnt;
      pl_total += pl_cnt;
      c1_total += c1_cnt;
      pl1_total += pl1_cnt;
      td_total += td_cnt;

      cerr << "\n";
      PrintInfo("loops: ", loop_total, loops_max);
      PrintInfo("degree tests: ", degree_total, tuples_max);

      PrintInfo("n-2 tests: ", n2_total, tuples_max);

      cerr << "ave sum len: ";
      if (n2_total == 0) 
         cerr << "--";
      else
         cerr << (to_double(sl_total)/to_double(n2_total));
      cerr << "\n";

      PrintInfo("f(1) tests: ", c1_total, tuples_max);

      cerr << "ave prod len: ";
      if (c1_total == 0) 
         cerr << "--";
      else
         cerr << (to_double(pl1_total)/to_double(c1_total));
      cerr << "\n";

      PrintInfo("f(0) tests: ", ct_total, tuples_max);

      cerr << "ave prod len: ";
      if (ct_total == 0) 
         cerr << "--";
      else
         cerr << (to_double(pl_total)/to_double(ct_total));
      cerr << "\n";

      PrintInfo("trial divs: ", td_total, tuples_max);
   }
}



static
void FindTrueFactors(vec_ZZX& factors, const ZZX& ff, 
                     const vec_ZZX& w, const ZZ& P, 
                     LocalInfoT& LocalInfo,
                     long verbose,
                     long bnd)
{
   ZZ_pBak bak;
   bak.save();
   ZZ_p::init(P);

   long r = w.length();

   vec_ZZ_pX W;
   W.SetLength(r);

   long i;
   for (i = 0; i < r; i++)
      conv(W[i], w[i]);


   ZZX f;

   f = ff;

   double t;
   t = GetTime();

   long k;

   k = 1;
   factors.SetLength(0);
   while (2*k <= W.length()) {
      if (k <= 2)
         CardinalitySearch(factors, f, W, LocalInfo, k, bnd, verbose);
      else
         CardinalitySearch1(factors, f, W, LocalInfo, k, bnd, verbose);
      k++;
   }

   append(factors, f);

   bak.restore();
}




void SFFactor(vec_ZZX& factors, const ZZX& ff, 
              long verbose,
              long bnd)

// input is primitive and square-free, with positive leading
// coefficient
{
   if (deg(ff) <= 1) {
      factors.SetLength(1);
      factors[0] = ff;
      if (verbose) {
         cerr << "*** SFFactor, trivial case 1.\n";
      }
      return;
   }

   // remove a factor of X, if necessary

   ZZX f;
   long xfac;
   long rev;

   double t;

   if (IsZero(ConstTerm(ff))) {
      RightShift(f, ff, 1);
      xfac = 1;
   }
   else {
      f = ff;
      xfac = 0;
   }

   // return a factor of X-1 if necessary

   long x1fac = 0;

   ZZ c1;
   SumCoeffs(c1, f);

   if (c1 == 0) {
      x1fac = 1;
      div(f, f, ZZX(1,1) - 1);
   }

   SumCoeffs(c1, f);

   if (deg(f) <= 1) {
      long r = 0;
      factors.SetLength(0);
      if (deg(f) > 0) {
         factors.SetLength(r+1);
         factors[r] = f;
         r++;
      }
      if (xfac) {
         factors.SetLength(r+1);
         SetX(factors[r]);
         r++;
      }

      if (x1fac) {
         factors.SetLength(r+1);
         factors[r] = ZZX(1,1) - 1;
         r++;
      }

      if (verbose) {
         cerr << "*** SFFactor: trivial case 2.\n";
      }

      return;
   }

   if (verbose) {
      cerr << "*** start SFFactor.\n";
   }

   // reverse f if this makes lead coefficient smaller

   ZZ t1, t2;

   abs(t1, LeadCoeff(f));
   abs(t2, ConstTerm(f));

   if (t1 > t2) {
      inplace_rev(f);
      rev = 1;
   }
   else 
      rev = 0;

   // obtain factorization modulo small primes

   if (verbose) {
      cerr << "factorization modulo small primes...\n";
      t = GetTime();
   }

   LocalInfoT LocalInfo;

   zz_pBak bak;
   bak.save();

   vec_zz_pX *spfactors =
       SmallPrimeFactorization(LocalInfo, f, verbose);

   if (!spfactors) {
      // f was found to be irreducible 

      bak.restore();

      if (verbose) {
         t = GetTime()-t;
         cerr << "small prime time: " << t << ", irreducible.\n";
      }

      if (rev)
         inplace_rev(f);

      long r = 0;

      factors.SetLength(r+1);
      factors[r] = f;
      r++;

      if (xfac) {
         factors.SetLength(r+1);
         SetX(factors[r]);
         r++;
      }

      if (x1fac) {
         factors.SetLength(r+1);
         factors[r] = ZZX(1,1) - 1;
         r++;
      }

      return;
   }

   if (verbose) {
      t = GetTime()-t;
      cerr << "small prime time: ";
      cerr << t << ", number of factors = " << spfactors->length() << "\n";
   }

   // prepare for Hensel lifting

   // first, calculate bit bound 

   long bnd1;
   long n = deg(f);
   long i;
   long e;
   ZZ P;
   long p;
   
   bnd1 = MaxBits(f) + (NumBits(n+1)+1)/2;

   if (!bnd || bnd1 < bnd)
      bnd = bnd1;

   i = n/2;
   while (!bit(LocalInfo.PossibleDegrees, i))
      i--;

   long lc_bnd = NumBits(LeadCoeff(f));

   long coeff_bnd = bnd + lc_bnd + i;

   long lift_bnd;

   lift_bnd = coeff_bnd + 15;  
   // +15 helps avoid trial divisions...can be any number >= 0

   lift_bnd = max(lift_bnd, bnd + lc_bnd + 2*NumBits(n) + ZZX_OVERLIFT);
   // facilitates "n-1" and "n-2" tests

   lift_bnd = max(lift_bnd, lc_bnd + NumBits(c1));
   // facilitates f(1) test

   lift_bnd += 2;
   // +2 needed to get inequalities right


   p = zz_p::modulus();

   e = long(double(lift_bnd)/(log(double(p))/log(double(2))));
   power(P, p, e);

   while (NumBits(P) <= lift_bnd) { 
      mul(P, P, p);
      e++;
   }

   if (verbose) {
      cerr << "lifting bound = " << lift_bnd << "\n";
      cerr << "Hensel lifting to exponent " << e << "...";
      t = GetTime();
   }

   // third, compute f1 so that it is monic and equal to f mod P

   ZZX f1;

   if (LeadCoeff(f) == 1)
      f1 = f;
   else if (LeadCoeff(f) == -1)
      negate(f1, f);
   else {
      rem(t1, LeadCoeff(f), P);
      if (sign(P) < 0)
         Error("whoops!!!");
      InvMod(t1, t1, P);
      f1.rep.SetLength(n+1);
      for (i = 0; i <= n; i++) {
         mul(t2, f.rep[i], t1);
         rem(f1.rep[i], t2, P);
      }
   }


   // Do Hensel lift

   vec_ZZX w;

   MultiLift(w, *spfactors, f1, e, verbose);


   if (verbose) {
      t = GetTime()-t;
      cerr << t << "\n";
   }

   // We're done with zz_p...restore

   delete spfactors;
   bak.restore();

   // search for true factors

   if (verbose) {
      cerr << "searching for true factors...\n";
      t = GetTime();
   }

   FindTrueFactors(factors, f, w, P, LocalInfo, 
                   verbose, coeff_bnd);

   if (verbose) {
      t = GetTime()-t;
      cerr << "factor search time " << t << "\n";
   }

   long r = factors.length();

   if (rev) {
      for (i = 0; i < r; i++) {
         inplace_rev(factors[i]);
         if (sign(LeadCoeff(factors[i])) < 0)
            negate(factors[i], factors[i]);
      }
   }

   if (xfac) {
      factors.SetLength(r+1);
      SetX(factors[r]);
      r++;
   }

   if (x1fac) {
      factors.SetLength(r+1);
      factors[r] = ZZX(1,1)-1;
      r++;
   }

   // that's it!!

   if (verbose) {
      cerr << "*** end SFFactor.  degree sequence:\n";
      for (i = 0; i < r; i++)
         cerr << deg(factors[i]) << " ";
      cerr << "\n";
   }
}





void factor(ZZ& c,
            vec_pair_ZZX_long& factors,
            const ZZX& f,
            long verbose,
            long bnd)

{
   ZZX ff = f;

   if (deg(ff) <= 0) {
      c = ConstTerm(ff);
      factors.SetLength(0);
      return;
   }

   content(c, ff);
   divide(ff, ff, c);

   long bnd1 = MaxBits(ff) + (NumBits(deg(ff)+1)+1)/2;
   if (!bnd || bnd > bnd1)
      bnd = bnd1;

   vec_pair_ZZX_long sfd;

   double t;

   if (verbose) { cerr << "square-free decomposition..."; t = GetTime(); }
   SquareFreeDecomp(sfd, ff);
   if (verbose) cerr << (GetTime()-t) << "\n";

   factors.SetLength(0);

   vec_ZZX x;

   long i, j;

   for (i = 0; i < sfd.length(); i++) {
      if (verbose) {
         cerr << "factoring multiplicity " << sfd[i].b
              << ", deg = " << deg(sfd[i].a) << "\n";
         t = GetTime();
      }

      SFFactor(x, sfd[i].a, verbose, bnd);

      if (verbose) {
         t = GetTime()-t;
         cerr << "total time for multiplicity " 
              << sfd[i].b << ": " << t << "\n";
      }

      for (j = 0; j < x.length(); j++)
         append(factors, cons(x[j], sfd[i].b));
   }
}

