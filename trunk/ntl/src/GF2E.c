

#include <NTL/GF2E.h>
#include <NTL/tools.h>


GF2EInfoT::GF2EInfoT(const GF2X& NewP)
{
   ref_count = 1;

   build(p, NewP);

   if (p.size == 1) {
      if (deg(p) <= NTL_BITS_PER_LONG/2)
         KarCross = 4;
      else
         KarCross = 8;
   }
   else if (p.size == 2)
      KarCross = 8;
   else if (p.size <= 5)
      KarCross = 4;
   else if (p.size == 6)
      KarCross = 3;
   else 
      KarCross = 2;


   if (p.size <= 1) {
      if (deg(p) <= NTL_BITS_PER_LONG/2)
         ModCross = 20;
      else
         ModCross = 40;
   }
   else if (p.size <= 2)
      ModCross = 75;
   else if (p.size <= 4)
      ModCross = 50;
   else
      ModCross = 25;

   if (p.size == 1) {
      if (deg(p) <= NTL_BITS_PER_LONG/2)
         DivCross = 100;
      else
         DivCross = 200;
   }
   else if (p.size == 2)
      DivCross = 400;
   else if (p.size <= 4)
      DivCross = 200;
   else if (p.size == 5)
      DivCross = 150;
   else if (p.size <= 13)
      DivCross = 100;
   else 
      DivCross = 75;

   ::power(cardinality, 2, p.n); 
}




GF2EInfoT *GF2EInfo = 0; 
typedef GF2EInfoT *GF2EInfoPtr;

#if ((defined (_THREAD_SAFE)) || (defined (_REENTRANT))) \
      && (!defined (COARSE_LOCKS))

pthread_rwlock_t GF2E_lock;

#endif


static 
void CopyPointer(GF2EInfoPtr& dst, GF2EInfoPtr src)
{
   if (src == dst) return;

   if (dst) {
      dst->ref_count--;

      if (dst->ref_count < 0) 
         Error("internal error: negative GF2EContext ref_count");

      if (dst->ref_count == 0) delete dst;
   }

   if (src) {
      src->ref_count++;

      if (src->ref_count < 0) 
         Error("internal error: GF2EContext ref_count overflow");
   }

   dst = src;
}
   



void GF2E::init(const GF2X& p)
{
   GF2EContext c(p);
   c.restore();
}


GF2EContext::GF2EContext(const GF2X& p)
{
   ptr = new GF2EInfoT(p);
}

GF2EContext::GF2EContext(const GF2EContext& a)
{
   ptr = 0;
   CopyPointer(ptr, a.ptr);
}

GF2EContext& GF2EContext::operator=(const GF2EContext& a)
{
   CopyPointer(ptr, a.ptr);
   return *this;
}


GF2EContext::~GF2EContext()
{
   CopyPointer(ptr, 0);
}

void GF2EContext::save()
{
   CopyPointer(ptr, GF2EInfo);
}

void GF2EContext::restore() const
{
#if (defined (_THREAD_SAFE)) || (defined (_REENTRANT))
   pthread_rwlock_wrlock (&GF2E_lock);
#endif

   CopyPointer(GF2EInfo, ptr);

#if (defined (_THREAD_SAFE)) || (defined (_REENTRANT))
   pthread_rwlock_unlock (&GF2E_lock);
#endif
}



GF2EBak::~GF2EBak()
{
   if (MustRestore)
      CopyPointer(GF2EInfo, ptr);

   CopyPointer(ptr, 0);
}

void GF2EBak::save()
{
   MustRestore = 1;
   CopyPointer(ptr, GF2EInfo);
}



void GF2EBak::restore()
{
   MustRestore = 0;

#if (defined (_THREAD_SAFE)) || (defined (_REENTRANT))
   pthread_rwlock_wrlock (&GF2E_lock);
#endif

   CopyPointer(GF2EInfo, ptr);

#if (defined (_THREAD_SAFE)) || (defined (_REENTRANT))
   pthread_rwlock_unlock (&GF2E_lock);
#endif
}



const GF2E& GF2E::zero()
{
   static GF2E z(GF2E_NoAlloc);
   return z;
}



istream& operator>>(istream& s, GF2E& x)
{
   GF2X y;

   s >> y;
   conv(x, y);

   return s;
}

void GF2EInfoT::div(GF2E& x, const GF2E& a, const GF2E& b) const
{
   GF2E t;

   inv(t, b);
   mul(x, a, t);
}

void GF2EInfoT::div(GF2E& x, GF2 a, const GF2E& b) const
{
   inv(x, b);
   mul(x, x, a);
}

void GF2EInfoT::div(GF2E& x, long a, const GF2E& b) const
{
   inv(x, b);
   mul(x, x, a);
}


void GF2EInfoT::inv(GF2E& x, const GF2E& a) const
{
   InvMod(x.rep, a.rep, p);
}

