/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Copyright RIA-LAAS/CNRS, 2010
 *+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * File:      landmarkAbstract.h
 * Project:   RT-Slam
 * Author:    Joan Sola
 *
 * Version control
 * ===============
 *
 *  $Id$
 *
 * Description
 * ============
 *
 *
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/**
 * \file landmarkAbstract.hpp
 * \author jsola@laas.fr
 * File defining the abstract landmark class
 * \ingroup rtslam
 */

#ifndef __LandmarkAbstract_H__
#define __LandmarkAbstract_H__

//#include "kernel/IdFactory.hpp"

#include "rtslam/rtSlam.hpp"

// include parents
#include "rtslam/mapAbstract.hpp"
#include "rtslam/mapManager.hpp"
#include "rtslam/mapObject.hpp"
#include "rtslam/descriptorAbstract.hpp"
//#include "rtslam/observationAbstract.hpp"
#include "rtslam/parents.hpp"

#include <boost/smart_ptr.hpp>

namespace jafar {
	namespace rtslam {
		using namespace std;


		// Forward declarations of children
		class ObservationAbstract;


		/** Base class for all landmarks defined in the module
		 * rtslam.
		 *
		 * @ingroup rtslam
		 */
		class LandmarkAbstract: public MapObject, public ChildOf<MapManagerAbstract> , public boost::enable_shared_from_this<
		    LandmarkAbstract>, public ParentOf<ObservationAbstract> {


				// define the function linkToParentMapManager().
		       ENABLE_LINK_TO_PARENT(MapManagerAbstract,MapManager,LandmarkAbstract)
				;
				// define the functions mapManagerPtr() and mapManager().
			ENABLE_ACCESS_TO_PARENT(MapManagerAbstract,mapManager)
				;
				// define the type ObservationList, and the function observationList().
			ENABLE_ACCESS_TO_CHILDREN(ObservationAbstract,Observation,observation)
				;

			public:
				/**
				 * constructor from map and size
				 */
			LandmarkAbstract(const map_ptr_t & _mapPtr, const size_t _size);
			LandmarkAbstract(const simulation_t dummy, const map_ptr_t & _mapPtr, const size_t _size);
		                /**
				 * Constructor by replacement: occupied the same filter state as a specified previous lmk. _icomp is the complementary memory, to be relaxed by the user.
				 */
			LandmarkAbstract(const map_ptr_t & _mapPtr, const landmark_ptr_t & _prevLmk, const size_t _size,jblas::ind_array & _icomp);

				/**
				 * Mandatory virtual destructor.
				 *
				 * We purposely kill all links from sensors to the observations depending on this landmark.
				 * This way, killing the landmark will kill all derived observations because
				 * all shared_ptr will be eliminated.
				 */
				virtual ~LandmarkAbstract();


				static IdFactory landmarkIds;
				void setId(){id(landmarkIds.getId());}

				enum geometry_t {
						POINT,
						LINE,
						PLANE,
						ELLIPSE
				} ;

				enum type_enum{
						PNT_EUC, PNT_AH
				};
				type_enum type;

			protected:
				geometry_t geomType;
			public:
				std::string categoryName() const {return "LANDMARK";}
				geometry_t getGeomType() const {return geomType;}
				virtual std::string typeName() const {return "Abstract";}

				descriptor_ptr_t descriptorPtr; ///< Landmark descriptor

				jblas::mat LNEW_lmk; ///<Jacobian comming from reparametrisation of old lmk wrt. new lmk

				//Reparametrize old Landmarks into new ones
				void reparametrize(const landmark_ptr_t & lmkDestPtr);
				void reparametrize(int size, vec &xNew, sym_mat &pNew);
				virtual vec reparametrize_func(const vec & lmk) = 0;
				virtual void reparametrize_func(const vec & lmk, vec & lnew, mat & LNEW_lmk) = 0;

				/**
				 * Size of the landmark state
				 */
				virtual size_t mySize() = 0;

				/**
				 * Size of the reparametrized landmark
				 */
				virtual size_t reparamSize() = 0;

				// Create a landmark descriptor
				virtual void setDescriptor(const descriptor_ptr_t & descPtr)
				{
					descriptorPtr = descPtr;
				}

				enum DecisionMethod {
						ANY,
						ALL,
						MAJORITY
				};

				/**
				 * Evaluate the landmark's need to die.
				 *
				 * This is a heuristic based on each observation statistics (the counters).
				 * \param dieMet the method defining the heuristic: ANY, ALL or MAJORITY
				 * \return \a true if the landmark should die.
				 */
				virtual bool needToDie(DecisionMethod dieMet = ANY);

				/**
				destroy the display data of itself and its children
				*/
				void destroyDisplay();
				/**
				 * Suicide
				 *
				 * We cut the linik to parent and the object naturally dies.
				 */
				void suicide();

				/**
				 * Evaluate the landmark's conditions for reparametrization
				 */
				virtual bool needToReparametrize(DecisionMethod repMethod = ALL);

				virtual void transferInfoLmk(landmark_ptr_t & lmkSourcePtr);

		};

	}
}

#endif // #ifndef __LandmarkAbstract_H__
