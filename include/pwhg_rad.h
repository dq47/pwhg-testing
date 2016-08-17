c -*- Fortran -*-

c rad_ubornidx: current index of underlying born
c rad_alr_list: list of alr's that share the current underlying born
c rad_alr_nlist: length of the above list
c rad_realidx: index in rad_alr_list of current alr
c rad_realalr: current alr

c rad_realreg: index of regular contribution in the array flst_regular
      integer rad_ubornidx,rad_alr_list(maxalr),rad_alr_nlist,
     #     rad_realidx,rad_realalr,rad_realreg,rho_idx,switch
c rad_kinreg: index in current kinematic region
c rad_nkinreg: number of kinematic regions
c     kinematic regions are numbered as:
c     1: initial state region
c     2 ... rad_nkinreg: final state regions with increasing
c                        emitter
c     rad_kinreg_on(rad_nkinreg): logical, entry j is true if there is a region
c     with rad_kinreg=j associated with current underlying born.
      integer rad_kinreg,rad_nkinreg
      logical rad_kinreg_on(nlegborn-1)
c rad_ncsiynormsmx: maximum number of csi-y subdivision when computing
c                   the upper bounds
c rad_ncsinorms,rad_nynorms: effective number of csi and y subdivisions
      integer rad_ncsiynormsmx
      parameter (rad_ncsiynormsmx=100)
      integer rad_ncsinorms,rad_nynorms
c 1 for Btilde event, 2 for remnant, 3 for regular
      integer rad_type
c Signed total, absolute value total, positive total and negative total
c obtained in the integration of btilde



c New variables added by DQ
c
c rho_idx: the value of rho that we are calculating (included in line 11 above)
c rhoweight = B1/(B1+B2) - used to calculate rho in Born.f and gen_index
c Bfact = B^\rho/(B1+B2) - after we have chosen \rho, Bfact is the Born cross section for the chosen value of rho
c Rfact = After we have chosen rho, Rfact is the amount we multiply our the gg part of our real cross section by so that we only include Real contributions with the same underlying Born colour flow (see FNO (4.56) - this is implementing new sum that we have in the numerator of the exponent)
c rhorweight(6) = array of the 6 planar amplitude weights in FNO (4.52)
c ggbornplanar1(2) = Born planar amplitudes for rho=1(2)
c switch = 0 if we are not near the collinear limit, and 1 if we are (as defined by [R(alr)-Rc(alr)]/R(alr) < \alpha_s (included in line 11 above)
c rsoft1 and rsoft2, like the soft limits of the Real matrix elements, EQ. 4.1.5 in my report
c
cccccccccccccccccccccccccccccccccc

      real * 8
     1     rad_totbtl,rad_etotbtl,
     2     rad_totabsbtl,rad_etotabsbtl,
     3     rad_totposbtl,rad_etotposbtl,
     4     rad_totnegbtl,rad_etotnegbtl,
     5     rad_totrm,rad_etotrm,
     6     rad_totbtlgen,rad_etotbtlgen,
     7     rad_totgen,rad_etotgen,
     8     rad_tot,rad_etot,rhoweight,Bfact,Rfact,rhorweight(6),ggbornplanar1,ggbornplanar2,
     9 	  rsoft1,rsoft2
c Grid of the upper bounds of the ratio (R*kn_jacreal/B)/upper_bounding function
c for each given kinematic region and underlying born
      real * 8 rad_csiynorms(rad_ncsiynormsmx,
     #     rad_ncsiynormsmx,nlegborn-1,maxprocborn)
c as above, on the whole grid, for each given underlying born
      real * 8 rad_norms(nlegborn-1,maxprocborn)
c value of btilde for each given underlying Born;
c filled after each final call to btilde.
      real * 8 rad_btilde_arr(maxprocborn)
c stores the sign of the above results in case the BOX is used
c with withnegweights=1.
      integer  rad_btilde_sign(maxprocborn)
c filled with contributions to real cross section after
c a call to sigreal_rad, according to the mapping rad_realalr
      real * 8 rad_real_arr(maxalr)
c filled with contributions of the real remnants after a call to
c sigremnants; rad_damp_rem_tot=Sum rad_damp_rem_arr
      real * 8 rad_damp_rem_arr(maxalr),rad_damp_rem_tot
c filled with contributions of the regular real graphs (graphs with no singular
c regions), filled after a call to sigremnants;
c rad_reg_tot=Sum rad_reg_arr
      real * 8 rad_reg_arr(maxprocreal),rad_reg_tot
c radiation variables in sigremnant call
      real * 8 rad_xradremn(3)
c user provided factor, to increase the upper bounding ratios
      real * 8 rad_normfact
c minimum pt-squared
      real * 8 rad_ptsqmin,rad_charmthr2,rad_bottomthr2
c LambdaLL for upper bounding coupling (see notes: running_coupling)
      real * 8 rad_lamll
c Hardest radiation kt2
      real * 8 rad_pt2max
c Branching ratio (useful to change xsecup properly when a decay is
c added a posteriori)
      real * 8 rad_branching
c Current event weight, needed when doing reweghting    
      real  * 8 rad_currentweight
      integer rad_iupperfsr,rad_iupperisr
      common/pwhg_rad/
     1     rad_totbtl,rad_etotbtl,
     2     rad_totabsbtl,rad_etotabsbtl,
     3     rad_totposbtl,rad_etotposbtl,
     4     rad_totnegbtl,rad_etotnegbtl,
     5     rad_totrm,rad_etotrm,
     6     rad_totbtlgen,rad_etotbtlgen,
     7     rad_totgen,rad_etotgen,
     8     rad_tot,rad_etot,
     9     rad_damp_rem_arr,rad_damp_rem_tot,rad_reg_arr,rad_reg_tot,
     1     rad_csiynorms,rad_norms,rad_btilde_arr,rad_real_arr,
     2     rad_normfact,rad_ptsqmin,rad_charmthr2,rad_bottomthr2,
     3     rad_lamll,rad_xradremn,rad_pt2max,
     4     rad_branching,rad_currentweight,rhoweight,rhorweight,Bfact,Rfact,ggbornplanar1,ggbornplanar2,
     5     rsoft1,rsoft2,
c     integers
     1     rad_ubornidx,rad_alr_list,rad_alr_nlist,
     2     rad_realidx,rad_realalr,rad_realreg,
     3     rad_kinreg,rad_nkinreg,
     4     rad_ncsinorms,rad_nynorms,rad_type,rad_btilde_sign,
     5     rad_iupperfsr,rad_iupperisr,rho_idx,switch,
c     logical
     6     rad_kinreg_on
      save /pwhg_rad/
