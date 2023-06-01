/**
 * @file <argos3/plugins/simulator/physics_engines/dynamics2d/dynamics2d_Polygon_model.h>
 *
 * @author Carlo Pinciroli - <ilpincy@gmail.com>
 */

#ifndef DYNAMICS2D_Polygon_MODEL_H
#define DYNAMICS2D_Polygon_MODEL_H

namespace argos {
   class CDynamics2DStretchableObjectModel;
   class CDynamics2DPolygonModel;
   class CPhysicsPolygonModel;
}

#include <argos3/plugins/simulator/physics_engines/dynamics2d/dynamics2d_stretchable_object_model.h>
#include <polygon_entity.h>

namespace argos {

   class CDynamics2DPolygonModel : public CDynamics2DStretchableObjectModel, public CPhysicsPolygonModel {

   public:

      CDynamics2DPolygonModel(CDynamics2DEngine& c_engine,
                          CPolygonEntity& c_entity);
      virtual ~CDynamics2DPolygonModel() {}

      void Resize(const CVector3& c_size);
   };

}

#endif