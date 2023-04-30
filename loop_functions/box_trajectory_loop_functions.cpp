#include "box_trajectory_loop_functions.h"
#include "buzz/buzzvm.h"

/****************************************/
/****************************************/

/*
 * To reduce the number of waypoints stored in memory,
 * consider two robot positions distinct if they are
 * at least MIN_DISTANCE away from each other
 * This constant is expressed in meters
 */
static const Real MIN_DISTANCE = 0.05f;
/* Convenience constant to avoid calculating the square root in PostStep() */
static const Real MIN_DISTANCE_SQUARED = MIN_DISTANCE * MIN_DISTANCE;

/****************************************/
/****************************************/

/**
 * Functor to put the stimulus in the Buzz VMs.
 */
struct PutStimuli : public CBuzzLoopFunctions::COperation {

   /** Constructor */
   PutStimuli(const argos::CVector3 vec_stimuli) : m_vecStimuli(vec_stimuli) {}
   
   /** The action happens here */
   virtual void operator()(const std::string& str_robot_id,buzzvm_t t_vm) {
      /* Set the values of the table 'stimulus' in the Buzz VM */
      BuzzTableOpen(t_vm, "obj_position");
      /*real_vec_stimuli.push_back(m_vecStimuli.GetX()); 
      real_vec_stimuli.push_back(m_vecStimuli.GetY());
      real_vec_stimuli.push_back(m_vecStimuli.GetZ()); */
      for(int i = 0; i < 3; ++i) {
         BuzzTablePut(t_vm, i, static_cast<float>(m_vecStimuli[i]));
      }
      BuzzTableClose(t_vm);
   }

   /** Calculated stimuli */
   const argos::CVector3 m_vecStimuli;
};


void CBoxTrajectoryLoopFunctions::Init(TConfigurationNode& t_tree) {
   /*
    * Go through all the robots in the environment
    * and create an entry in the waypoint map for each of them
    */
   /* Get the map of all foot-bots from the space */
   CBuzzLoopFunctions::Init(t_tree);
   GetNodeAttribute(t_tree, "outfile", m_strOutFile);
   CSpace::TMapPerType& tFBMap = GetSpace().GetEntitiesByType("box");
   /* Go through them */
   for(CSpace::TMapPerType::iterator it = tFBMap.begin();
       it != tFBMap.end();
       ++it) {
      /* Create a pointer to the current object */
      CBoxEntity* pcFB = any_cast<CBoxEntity*>(it->second);
      /* Create a waypoint vector */
      m_tWaypoints[pcFB] = std::vector<CVector3>();
      /* Add the initial position of the foot-bot */
      m_tWaypoints[pcFB].push_back(pcFB->GetEmbodiedEntity().GetOriginAnchor().Position);
      BuzzForeachVM(PutStimuli(pcFB->GetEmbodiedEntity().GetOriginAnchor().Position));
      }

   m_cOutFile.open(m_strOutFile.c_str(),
                  std::ofstream::out | std::ofstream::trunc);
}

/****************************************/
/****************************************/

void CBoxTrajectoryLoopFunctions::Reset() {
   /*
    * Clear all the waypoint vectors
    */
   /* Get the map of all foot-bots from the space */
   CSpace::TMapPerType& tFBMap = GetSpace().GetEntitiesByType("box");
   /* Go through them */
   for(CSpace::TMapPerType::iterator it = tFBMap.begin();
       it != tFBMap.end();
       ++it) {
      /* Create a pointer to the current foot-bot */
      CBoxEntity* pcFB = any_cast<CBoxEntity*>(it->second);
      /* Clear the waypoint vector */
      m_tWaypoints[pcFB].clear();
      /* Add the initial position of the foot-bot */
      m_tWaypoints[pcFB].push_back(pcFB->GetEmbodiedEntity().GetOriginAnchor().Position);
      BuzzForeachVM(PutStimuli(pcFB->GetEmbodiedEntity().GetOriginAnchor().Position));
   }

   m_cOutFile.open(m_strOutFile.c_str(),
                   std::ofstream::out | std::ofstream::trunc);
}

/****************************************/
/****************************************/

void CBoxTrajectoryLoopFunctions::PostStep() {
   /* Get the map of all foot-bots from the space */
   CSpace::TMapPerType& tFBMap = GetSpace().GetEntitiesByType("box");
   /* Go through them */
   for(CSpace::TMapPerType::iterator it = tFBMap.begin();
       it != tFBMap.end();
       ++it) {
      /* Create a pointer to the current foot-bot */
      CBoxEntity* pcFB = any_cast<CBoxEntity*>(it->second);
      /* Add the current position of the foot-bot if it's sufficiently far from the last */
      if(SquareDistance(pcFB->GetEmbodiedEntity().GetOriginAnchor().Position,
                        m_tWaypoints[pcFB].back()) > MIN_DISTANCE_SQUARED) {
         m_tWaypoints[pcFB].push_back(pcFB->GetEmbodiedEntity().GetOriginAnchor().Position);
      }
      BuzzForeachVM(PutStimuli(pcFB->GetEmbodiedEntity().GetOriginAnchor().Position));
   }

   CSpace::TMapPerType& mapFootBots = GetSpace().GetEntitiesByType("foot-bot");
   for(CSpace::TMapPerType::iterator it = mapFootBots.begin();
       it != mapFootBots.end();
       ++it) {
      /* Create a pointer to the current foot-bot */
      CFootBotEntity* pcFB = any_cast<CFootBotEntity*>(it->second);
      m_cOutFile << pcFB->GetId() << "\t"
                 << pcFB->GetEmbodiedEntity().GetOriginAnchor().Position << "\t";
   }
   m_cOutFile << std::endl;
}

void CTrajectoryLoopFunctions::Destroy() {
   m_cOutFile.close();
}

/****************************************/
/****************************************/
void CBoxTrajectoryLoopFunctions::BuzzBytecodeUpdated() {
   /* Convey the stimuli to every robot */
   /*BuzzForeachVM(PutStimuli(m_vecStimuli));*/
}
/****************************************/
/****************************************/

REGISTER_LOOP_FUNCTIONS(CBoxTrajectoryLoopFunctions, "box_trajectory_loop_functions")
