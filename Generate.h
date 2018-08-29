//
//  File:       Generate.h
//
//  Function:   Various sample/number generators
//
//  Copyright:  Andrew Willmott, 2018
//

#ifndef GENERATE_H
#define GENERATE_H

#include <math.h>
#include <stdint.h>

namespace DistLib
{
    //--------------------------------------------------------------------------
    // LCG
    //--------------------------------------------------------------------------

    struct cLCG
    {
        uint32_t mState = 0x12345678;

        cLCG() {};
        cLCG(uint32_t seed) : mState(seed) {}

        uint32_t Next();        ///< Explicit next number in sequence
        operator uint32_t();    ///< Return next number in sequence in uint32_t context
    };


    //--------------------------------------------------------------------------
    // PCG
    //--------------------------------------------------------------------------

    struct cPCG
    // See http://www.pcg-random.org
    {
        uint64_t mState = 0x853c49e6748fea9bULL;
        uint64_t mInc   = 0xda3e39cb94b95bdbULL;

        cPCG() {}
        cPCG(uint64_t initstate, uint64_t initseq);

        uint32_t Next();        ///< Explicit next number in sequence
        operator uint32_t();    ///< Return next number in sequence in uint32_t context
    };


    //--------------------------------------------------------------------------
    // Halton
    //--------------------------------------------------------------------------

    float    HaltonFloat (int i, int b); ///< return term i of the base b Halton sequence
    uint32_t HaltonUInt32(int i, int b); ///< return term i of the base b Halton sequence as fraction of UINT32_MAX

    float    Halton2Float (int i);   ///< return term i of the base 2 Halton sequence
    uint32_t Halton2UInt32(int i);   ///< return term i of the base 2 Halton sequence as fraction of UINT32_MAX

    struct cHalton2
    /// This calculates the 2D Halton sequence incrementally, faster per-call than HaltonFloat(i, 2/3).
    {
        float    mU[2] = { 0.0f, 0.0f };

        uint32_t mBase2 = 0;
        uint32_t mBase3 = 0;
        
        int  Next ();       ///< Advance to next point in the sequence. Returns the index of this point.
        void Set  (int n);  ///< Jump directly to term 'n' of the sequence
        int operator++() { return Next(); }  ///< Synonym for Next().
    };
    
    struct cHalton3
    /// This calculates the 3D Halton sequence incrementally, faster per-call than HaltonFloat(i, 2/3/5).
    {
        float    mU[3] = { 0.0f, 0.0f, 0.0f };

        uint32_t mBase2 = 0;
        uint32_t mBase3 = 0;
        uint32_t mBase5 = 0;
        
        int  Next ();       ///< Advance to next point in the sequence. Returns the index of this point.
        void Set  (int n);  ///< Jump directly to term 'n' of the sequence
        int operator++() { return Next(); }  ///< Synonym for Next().
    };

    struct cHalton2U
    /// This calculates the 2D Halton sequence incrementally, faster per call than HaltonUInt32.
    {
        uint32_t mU[2] = { 0 };     ///< Current sample point

        uint32_t mBase2 = 0;
        uint32_t mBase3 = 0;
        
        int  Next ();       ///< Advance to next point in the sequence. Returns the index of this point.
        void Set  (int n);  ///< Jump directly to term 'n' of the sequence
        int operator++() { return Next(); }  ///< Synonym for Next().
    };

    struct cHalton3U
    /// This calculates the 3D Halton sequence incrementally, faster per-call than HaltonUInt32(i, 2/3/5).
    {
        uint32_t mU[3] = { 0 };     ///< Current sample point
        
        uint32_t mBase2 = 0;
        uint32_t mBase3 = 0;
        uint32_t mBase5 = 0;
        
        int  Next ();       ///< Advance to next point in the sequence. Returns the index of this point.
        void Set  (int n);  ///< Jump directly to term 'n' of the sequence
        int  operator++() { return Next(); } ///< Synonym for Next().
    };

    
    //--------------------------------------------------------------------------
    // Fibonacci/Golden Spiral
    //--------------------------------------------------------------------------
    
    float    GoldenFloat (int i); ///< return term i of the golden angle sequence as float
    uint32_t GoldenUInt32(int i); ///< return term i of the golden angle sequence as fraction of UINT32_MAX

    struct cGolden2U
    {
        uint32_t mU[2] = { 0 };     ///< Current sample point
        uint32_t mStep;

        cGolden2U(int numSamples);
        
        void Set(int n);   ///< Jump directly to term 'n' of the sequence
        void operator++(); ///< Advance to next point in the sequence.
    };



    // --- Inlines -------------------------------------------------------------

    inline float Halton2Float(int i)
    {
        uint32_t u = Halton2UInt32(i);
        
        uint32_t resultU(0x3f800000 | (u >> 9));
        float result = ((float&) resultU);
        return result - 1.0f;
    }

    inline uint32_t Halton2UInt32(int a)
    {
        uint32_t b;

        b = ((a & 0x55555555) << 1)  | ((a & 0xAAAAAAAA) >> 1);
        a = ((b & 0x33333333) << 2)  | ((b & 0xCCCCCCCC) >> 2);
        b = ((a & 0x0F0F0F0F) << 4)  | ((a & 0xF0F0F0F0) >> 4);
        a = ((b & 0x00FF00FF) << 8)  | ((b & 0xFF00FF00) >> 8);
        b = ((a & 0x0000FFFF) << 16) | ((a & 0xFFFF0000) >> 16);

        return b;
    }

    inline uint32_t cLCG::Next()
    {
        uint32_t current(mState);
        mState = uint32_t(mState * uint64_t(1103515245) + 12345);
        return current;
    }

    inline cLCG::operator uint32_t()
    {
        return Next();
    }

    inline cPCG::cPCG(uint64_t initstate, uint64_t initseq) : mState(0), mInc((initseq << 1u) | 1u)
    {
        Next();
        mState += initstate;
        Next();
    }

    inline uint32_t cPCG::Next()
    {
        uint64_t oldstate = mState;
        mState = oldstate * 6364136223846793005ULL + mInc;

        uint32_t xorshifted = uint32_t(((oldstate >> 18u) ^ oldstate) >> 27u);
        uint32_t rot = oldstate >> 59u;

        return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
    }

    inline cPCG::operator uint32_t()
    {
        return Next();
    }

    const float    kGoldenF32 = 0.5f * (sqrt(5.0f) - 1.0f);
    const uint32_t kGoldenU32 = uint32_t(UINT32_MAX * kGoldenF32);

    inline float GoldenFloat(int i)
    {
        float f = i * kGoldenF32;
        return f - floorf(f);
    }

    inline uint32_t GoldenUInt32(int i)
    {
        return uint32_t(i * kGoldenU32);
    }


    inline cGolden2U::cGolden2U(int numSamples) :
        mStep(UINT32_MAX / numSamples)
    {
        mU[1] = mStep / 2;
    }

    inline void cGolden2U::Set(int i)
    {
        mU[0] = uint32_t(i * kGoldenU32);
        mU[1] = i * mStep + mStep / 2;
    }

    inline void cGolden2U::operator++()
    {
        mU[0] += kGoldenU32;
        mU[1] += mStep;
    }
}

#endif
