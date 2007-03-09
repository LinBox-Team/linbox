/* -*- mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
// ======================================================================= //
// Time-stamp: <09 Mar 07 20:20:11 Jean-Guillaume.Dumas@imag.fr> 
// ======================================================================= //
#ifndef __LINBOX_RATIONAL_CRA_H
#define __LINBOX_RATIONAL_CRA_H

#include "linbox/field/PID-integer.h"
#include "linbox/algorithms/cra-full-multip.h"

namespace LinBox {

//     template<class T, template <class T> class Container>
//     std::ostream& operator<< (std::ostream& o, const Container<T>& C) {
//         for(typename Container<T>::const_iterator refs =  C.begin();
//             refs != C.end() ;
//             ++refs )
//             o << (*refs) << " " ;
//         return o << std::endl;
//     }

    template<class Domain_Type>
    struct FullMultipRatCRA : public virtual FullMultipCRA<Domain_Type> {
        typedef Domain_Type				Domain;
        typedef FullMultipCRA<Domain> 			Father_t;
        typedef typename Father_t::DomainElement 	DomainElement;
        typedef FullMultipRatCRA<Domain>		Self_t;
        PID_integer _ZZ;
    public:

        using Father_t::RadixSizes_;
        using Father_t::RadixResidues_; 
        using Father_t::RadixPrimeProd_;
        using Father_t::RadixOccupancy_;
        

        FullMultipRatCRA(const double BOUND) : Father_t(BOUND) {}

		
        template<template<class T> class Vect>
        Vect<Integer>& result (Vect<Integer> &num, Integer& den){
            num.resize( (Father_t::RadixResidues_.front()).size() );
            std::vector< LazyProduct >::iterator 			_mod_it = Father_t::RadixPrimeProd_.begin();
            std::vector< std::vector< Integer > >::iterator _tab_it = Father_t::RadixResidues_.begin();
            std::vector< bool >::iterator    				_occ_it = Father_t::RadixOccupancy_.begin();
            LazyProduct Product;
            for( ; _occ_it != Father_t::RadixOccupancy_.end() ; ++_mod_it, ++_tab_it, ++_occ_it) {
                if (*_occ_it) {
                    Product = *_mod_it;
                    std::vector<Integer>::iterator t0_it = num.begin();
                    std::vector<Integer>::iterator t_it = _tab_it->begin();
                    if (++_occ_it == Father_t::RadixOccupancy_.end()) {
                        den = 1;
                        Integer s, nd; _ZZ.sqrt(s, _mod_it->operator()());
                        for( ; t0_it != num.end(); ++t0_it, ++t_it) {
                            iterativeratrecon(*t0_it = *t_it, nd, den, _mod_it->operator()(), s);
                            if (nd > 1) {
                                std::vector<Integer>::iterator  t02 = num.begin();
                                for( ; t02 != t0_it ; ++t02)
                                    *t02 *= nd;
                                den *= nd;
                            }
                        }
                        return num;
                    } else {
                        for( ; t0_it != num.end(); ++t0_it, ++t_it)
                            *t0_it  = *t_it;
                        ++_mod_it; ++_tab_it; 
                        break;
                    }
                }
            }
            for( ; _occ_it != Father_t::RadixOccupancy_.end() ; ++_mod_it, ++_tab_it, ++_occ_it) {
                if (*_occ_it) {
                    std::vector<Integer>::iterator t0_it = num.begin();
                    std::vector<Integer>::const_iterator t_it = _tab_it->begin();
                    for( ; t0_it != num.end(); ++t0_it, ++t_it)
                        this->normalizesmallbigreconstruct(*t0_it, Product(), *t_it, _mod_it->operator()() );
                    Product.mulin(*_mod_it);
                }
            }
            den = 1;
            Integer s, nd; _ZZ.sqrt(s, Product.operator()());
            std::vector<Integer>::iterator t0_it = num.begin();
            for( ; t0_it != num.end(); ++t0_it) {
                iterativeratrecon(*t0_it, nd, den, Product.operator()(), s);
                if (nd > 1) {
                    std::vector<Integer>::iterator  t02 = num.begin();
                    for( ; t02 != t0_it ; ++t02)
                        *t02 *= nd;
                    den *= nd;
                }
            }
            return num;
        }
		

    protected:
        Integer& iterativeratrecon(Integer& u1, Integer& new_den, const Integer& old_den, const Integer& m1, const Integer& s) {
            Integer a;
            _ZZ.reconstructRational(a, new_den, u1*=old_den, m1, s);
            return u1=a;
        }
    };
}

#endif
