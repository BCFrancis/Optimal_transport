/**
 * @file <argos3/plugins/simulator/entities/polygon_entity.cpp>
 *
 * @author Carlo Pinciroli - <ilpincy@gmail.com>
 */

#include "polygon_entity.h"
#include <argos3/core/utility/math/matrix/rotationmatrix3.h>
#include <argos3/core/simulator/space/space.h>
#include <argos3/core/simulator/simulator.h>
#include <argos3/plugins/simulator/media/led_medium.h>

namespace argos {

   /****************************************/
   /****************************************/

   CPolygonEntity::CPolygonEntity():
      CComposableEntity(nullptr),
      m_pcEmbodiedEntity(nullptr),
      m_pcLEDEquippedEntity(nullptr),
      m_fMass(1.0f),
      m_pcLEDMedium(nullptr) {}

   /****************************************/
   /****************************************/

   CPolygonEntity::CPolygonEntity(const std::string& str_id,
                          const CVector3& c_position,
                          const CQuaternion& c_orientation,
                          bool b_movable,
                          const float c_points[][2],
                          Real f_mass) :
      CComposableEntity(nullptr, str_id),
      m_pcEmbodiedEntity(
         new CEmbodiedEntity(this,
                             "body_0",
                             c_position,
                             c_orientation,
                             b_movable)),
      m_pcLEDEquippedEntity(
         new CLEDEquippedEntity(this,
                                "leds_0")),
      m_cPoints(c_points),
      m_fMass(f_mass) {
      AddComponent(*m_pcEmbodiedEntity);
      AddComponent(*m_pcLEDEquippedEntity);
   }

   /****************************************/
   /****************************************/

   void CPolygonEntity::Init(TConfigurationNode& t_tree) {
      try {
         /* Init parent */
         CComposableEntity::Init(t_tree);
         /* Parse XML to get the size */
         GetNodeAttribute(t_tree, "points", m_cPoints);
         /* Parse XML to get the movable attribute */         
         bool bMovable;
         GetNodeAttribute(t_tree, "movable", bMovable);
         if(bMovable) {
            /* Parse XML to get the mass */
            GetNodeAttribute(t_tree, "mass", m_fMass);
         }
         else {
            m_fMass = 0.0f;
         }
         /* Create embodied entity using parsed data */
         m_pcEmbodiedEntity = new CEmbodiedEntity(this);
         AddComponent(*m_pcEmbodiedEntity);
         m_pcEmbodiedEntity->Init(GetNode(t_tree, "body"));
         m_pcEmbodiedEntity->SetMovable(bMovable);
         /* Init LED equipped entity component */
         m_pcLEDEquippedEntity = new CLEDEquippedEntity(this);
         AddComponent(*m_pcLEDEquippedEntity);
         if(NodeExists(t_tree, "leds")) {
            /* Create LED equipped entity
             * NOTE: the LEDs are not added to the medium yet
             */
            m_pcLEDEquippedEntity->Init(GetNode(t_tree, "leds"));
            /* Add the LEDs to the medium */
            std::string strMedium;
            GetNodeAttribute(GetNode(t_tree, "leds"), "medium", strMedium);
            m_pcLEDMedium = &CSimulator::GetInstance().GetMedium<CLEDMedium>(strMedium);
            m_pcLEDEquippedEntity->SetMedium(*m_pcLEDMedium);
            m_pcLEDEquippedEntity->Enable();
         }
         UpdateComponents();
      }
      catch(CARGoSException& ex) {
         THROW_ARGOSEXCEPTION_NESTED("Failed to initialize Polygon entity \"" << GetId() << "\".", ex);
      }
   }

   /****************************************/
   /****************************************/

   void CPolygonEntity::Reset() {
      /* Reset all components */
      CComposableEntity::Reset();
      /* Update components */
      UpdateComponents();
   }

   /****************************************/
   /****************************************/

   void CPolygonEntity::EnableLEDs(CLEDMedium& c_medium) {
      m_pcLEDMedium = &c_medium;
      m_pcLEDEquippedEntity->SetMedium(*m_pcLEDMedium);
      m_pcLEDEquippedEntity->Enable();
   }

   /****************************************/
   /****************************************/

   void CPolygonEntity::DisableLEDs() {
      m_pcLEDEquippedEntity->Disable();
   }
   
   /****************************************/
   /****************************************/

   void CPolygonEntity::AddLED(const CVector3& c_offset,
                           const CColor& c_color) {
      m_pcLEDEquippedEntity->AddLED(c_offset,
                                    GetEmbodiedEntity().GetOriginAnchor(),
                                    c_color);
      UpdateComponents();
   }

   /****************************************/
   /****************************************/

   REGISTER_ENTITY(CPolygonEntity,
                   "polygon",
                   "Carlo Pinciroli [ilpincy@gmail.com]",
                   "1.0",
                   "A 3D Polygon.",
                   "The Polygon entity can be used to model walls, obstacles or Polygon-shaped grippable\n"
                   "objects. It can be movable or not. A movable object can be pushed and gripped.\n"
                   "An unmovable object is pretty much like a wall.\n\n"
                   "REQUIRED XML CONFIGURATION\n\n"
                   "To declare an unmovable object (i.e., a wall) you need the following:\n\n"
                   "  <arena ...>\n"
                   "    ...\n"
                   "    <polygon id=\"polygon1\" points=\"[[0.75,0.1],[0.5,0.6],[0.0,0.1]]\" movable=\"false\">\n"
                   "      <body position=\"0.4,2.3,0\" orientation=\"45,0,0\" />\n"
                   "    </polygon>\n"
                   "    ...\n"
                   "  </arena>\n\n"
                   "To declare a movable object you need the following:\n\n"
                   "  <arena ...>\n"
                   "    ...\n"
                   "    <polygon id=\"polygon1\" points=\"[[0.75,0.1],[0.5,0.6],[0.0,0.1]\" movable=\"true\" mass=\"2.5\">\n"
                   "      <body position=\"0.4,2.3,0\" orientation=\"45,0,0\" />\n"
                   "    </polygon>\n"
                   "    ...\n"
                   "  </arena>\n\n"
                   "The 'id' attribute is necessary and must be unique among the entities. If two\n"
                   "entities share the same id, initialization aborts.\n"
                   "The 'points' attribute specifies the location of the polygon vertices\n"
                   "When you add a polygon, imagine it initially unrotated and\n"
                   "centered in the origin. The points, then, corresponds to the locations along the X and \n"
                   "Y axes.\n"
                   "The 'movable' attribute specifies whether or not the object is movable. When\n"
                   "set to 'false', the object is unmovable: if another object pushes against it,\n"
                   "the polygon won't move. When the attribute is set to 'true', the polygon is movable\n"
                   "upon pushing or gripping. When an object is movable, the 'mass' attribute is\n"
                   "required.\n"
                   "The 'mass' attribute quantifies the mass of the polygon in kg.\n"
                   "The 'body/position' attribute specifies the position of the base of the polygon in\n"
                   "the arena. The three values are in the X,Y,Z order.\n"
                   "The 'body/orientation' attribute specifies the orientation of the 3D polygon. All\n"
                   "rotations are performed with respect to the center of mass. The order of the\n"
                   "angles is Z,Y,X, which means that the first number corresponds to the rotation\n"
                   "around the Z axis, the second around Y and the last around X. This reflects\n"
                   "the internal convention used in ARGoS, in which rotations are performed in\n"
                   "that order. Angles are expressed in degrees.\n\n"
                   "OPTIONAL XML CONFIGURATION\n\n"
                   "It is possible to add any number of colored LEDs to the polygon. In this way,\n"
                   "the polygon is visible with a robot camera. The position and color of the\n"
                   "LEDs is specified with the following syntax:\n\n"
                   "  <arena ...>\n"
                   "    ...\n"
                   "    <polygon id=\"polygon1\" points=\"[[0.75,0.1],[0.5,0.6],[0.0,0.1]\" movable=\"true\" mass=\"2.5\">\n"
                   "      <body position=\"0.4,2.3,0\" orientation=\"45,0,0\" />\n"
                   "      <leds medium=\"id_of_led_medium\">\n"
                   "        <led offset=\" 0.15, 0.15,0.15\" anchor=\"origin\" color=\"white\" />\n"
                   "        <led offset=\"-0.15, 0.15,0\"    anchor=\"origin\" color=\"red\"   />\n"
                   "        <led offset=\" 0.15, 0.15,0\"    anchor=\"origin\" color=\"blue\"  />\n"
                   "        <led offset=\" 0.15,-0.15,0\"    anchor=\"origin\" color=\"green\" />\n"
                   "      </leds>\n"
                   "    </polygon>\n"
                   "    ...\n"
                   "  </arena>\n\n"
                   "In the example, four LEDs are added to the polygon. The LEDs have\n"
                   "different colors and are located one on the top and three\n"
                   "around the polygon. The LEDs are managed by the LED medium declared in\n"
                   "the <media> section of the configuration file with id \"id_of_led_medium\"",
                   "Usable"
      );

   /****************************************/
   /****************************************/

   REGISTER_STANDARD_SPACE_OPERATIONS_ON_COMPOSABLE(CPolygonEntity);

   /****************************************/
   /****************************************/

}