/**
 * @file <argos3/plugins/simulator/physics_engines/dynamics2d/dynamics2d_polygon_model.cpp>
 *
 * @author Carlo Pinciroli - <ilpincy@gmail.com>
 */

#include "dynamics2d_polygon_model.h"
#include "dynamics2d_gripping.h"
#include "dynamics2d_engine.h"

namespace argos {

   /****************************************/
   /****************************************/

   CDynamics2DpolygonModel::CDynamics2DpolygonModel(CDynamics2DEngine& c_engine,
                                            CPolygonEntity& c_entity) :
      CDynamics2DStretchableObjectModel(c_engine, c_entity) {

      /* Create a polygonal object in the physics space */
      /* Start defining the vertices
         NOTE: points must be defined in a clockwise winding
      */
      cpVect tVertices[] = c_entity.GetPoints();
      const CVector3& cPosition = GetEmbodiedEntity().GetOriginAnchor().Position;
      CRadians cXAngle, cYAngle, cZAngle;
      GetEmbodiedEntity().GetOriginAnchor().Orientation.ToEulerAngles(cZAngle, cYAngle, cXAngle);
      /*
       * Create body and shapes
       */
      cpBody* ptBody;
      if(GetEmbodiedEntity().IsMovable()) {
         /* The polygon is movable */
         SetMass(c_entity.GetMass());
         /* Create the body */
         ptBody =
            cpSpaceAddBody(GetDynamics2DEngine().GetPhysicsSpace(),
                           cpBodyNew(GetMass(),
                                     cpMomentForPoly(GetMass(),
                                                     std::sizeof(tVertices)/std::sizeof(tVertices[0]),
                                                     tVertices,
                                                     cpvzero)));
         ptBody->p = cpv(cPosition.GetX(), cPosition.GetY());
         cpBodySetAngle(ptBody, cZAngle.GetValue());
         /* Create the shape */
         cpShape* ptShape =
            cpSpaceAddShape(GetDynamics2DEngine().GetPhysicsSpace(),
                            cpPolyShapeNew(ptBody,
                                           std::sizeof(tVertices)/std::sizeof(tVertices[0]),
                                           tVertices,
                                           cpvzero));
         ptShape->e = 0.0; // no elasticity
         ptShape->u = 0.7; // lots contact friction to help pushing
         /* The shape is grippable */
         SetGrippable(new CDynamics2DGrippable(GetEmbodiedEntity(),
                                               ptShape));
         /* Set the body so that the default methods work as expected */
         SetBody(ptBody, c_entity.GetSize().GetZ());
         /* Friction with ground */
         SetLinearFriction(0.0f, c_engine.GetBoxLinearFriction());
         SetAngularFriction(0.0f, c_engine.GetBoxAngularFriction());
      }
      else {
         /* The polygon is not movable */
         /* Create a static body */
         ptBody = cpBodyNewStatic();
         ptBody->p = cpv(cPosition.GetX(), cPosition.GetY());
         cpBodySetAngle(ptBody, cZAngle.GetValue());
         /* Create the shape */
         cpShape* ptShape =
            cpSpaceAddShape(GetDynamics2DEngine().GetPhysicsSpace(),
                            cpPolyShapeNew(ptBody,
                                           std::sizeof(tVertices)/std::sizeof(tVertices[0]),
                                           tVertices,
                                           cpvzero));
         ptShape->e = 0.0; // No elasticity
         ptShape->u = 0.1; // Little contact friction to help sliding away
         /* This shape is normal (not grippable, not gripper) */
         ptShape->collision_type = CDynamics2DEngine::SHAPE_NORMAL;
         /* Set the body so that the default methods work as expected */
         SetBody(ptBody, c_entity.GetSize().GetZ());
      }
   }

   
   /****************************************/
   /****************************************/

   REGISTER_STANDARD_DYNAMICS2D_OPERATIONS_ON_ENTITY(CPolygonEntity, CDynamics2DPolygonModel);

   /****************************************/
   /****************************************/

}