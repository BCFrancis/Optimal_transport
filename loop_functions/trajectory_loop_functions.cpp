#include "trajectory_loop_functions.h"
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
   PutStimuli(const std::vector<CVector3>& vec_stimuli) : m_vecStimuli(vec_stimuli) {}
   
   /** The action happens here */
   virtual void operator()(const std::string& str_robot_id,buzzvm_t t_vm) {
      /* Set the values of the table 'stimulus' in the Buzz VM */
      std::vector<Real> real_vec_stimuli;
      BuzzTableOpen(t_vm, "obj_position");
      /*real_vec_stimuli.push_back(m_vecStimuli.GetX()); 
      real_vec_stimuli.push_back(m_vecStimuli.GetY());
      real_vec_stimuli.push_back(m_vecStimuli.GetZ()); */
      for(int i = 0; i < m_vecStimuli.size(); ++i) {
         BuzzTablePut(t_vm, i, static_cast<float>(m_vecStimuli[i]));
      }
      BuzzTableClose(t_vm);
   }

   /** Calculated stimuli */
   const std::vector<CVector3>& m_vecStimuli;
};


void CTrajectoryLoopFunctions::Init(TConfigurationNode& t_tree) {
   /*
    * Go through all the robots in the environment
    * and create an entry in the waypoint map for each of them
    */
   /* Get the map of all foot-bots from the space */
   CBuzzLoopFunctions::Init(t_tree);
   CSpace::TMapPerType& tFBMap = GetSpace().GetEntitiesByType("cylinder");
   /* Go through them */
   for(CSpace::TMapPerType::iterator it = tFBMap.begin();
       it != tFBMap.end();
       ++it) {
      /* Create a pointer to the current object */
      CCylinderEntity* pcFB = any_cast<CCylinderEntity*>(it->second);
      /* Create a waypoint vector */
      m_tWaypoints[pcFB] = std::vector<CVector3>();
      /* Add the initial position of the foot-bot */
      m_tWaypoints[pcFB].push_back(pcFB->GetEmbodiedEntity().GetOriginAnchor().Position);
   }
}

/****************************************/
/****************************************/

void CTrajectoryLoopFunctions::Reset() {
   /*
    * Clear all the waypoint vectors
    */
   /* Get the map of all foot-bots from the space */
   CSpace::TMapPerType& tFBMap = GetSpace().GetEntitiesByType("cylinder");
   /* Go through them */
   for(CSpace::TMapPerType::iterator it = tFBMap.begin();
       it != tFBMap.end();
       ++it) {
      /* Create a pointer to the current foot-bot */
      CCylinderEntity* pcFB = any_cast<CCylinderEntity*>(it->second);
      /* Clear the waypoint vector */
      m_tWaypoints[pcFB].clear();
      /* Add the initial position of the foot-bot */
      m_tWaypoints[pcFB].push_back(pcFB->GetEmbodiedEntity().GetOriginAnchor().Position);
   }
}

/****************************************/
/****************************************/

void CTrajectoryLoopFunctions::PostStep() {
   /* Get the map of all foot-bots from the space */
   CSpace::TMapPerType& tFBMap = GetSpace().GetEntitiesByType("cylinder");
   /* Go through them */
   for(CSpace::TMapPerType::iterator it = tFBMap.begin();
       it != tFBMap.end();
       ++it) {
      /* Create a pointer to the current foot-bot */
      CCylinderEntity* pcFB = any_cast<CCylinderEntity*>(it->second);
      /* Add the current position of the foot-bot if it's sufficiently far from the last */
      if(SquareDistance(pcFB->GetEmbodiedEntity().GetOriginAnchor().Position,
                        m_tWaypoints[pcFB].back()) > MIN_DISTANCE_SQUARED) {
         m_tWaypoints[pcFB].push_back(pcFB->GetEmbodiedEntity().GetOriginAnchor().Position);
      }
   }
   BuzzForeachVM(PutStimuli(m_tWaypoints));

}
/****************************************/
/****************************************/
void CTrajectoryLoopFunctions::BuzzBytecodeUpdated() {
   /* Convey the stimuli to every robot */
   /*BuzzForeachVM(PutStimuli(m_vecStimuli));*/
}
/****************************************/
/****************************************/

REGISTER_LOOP_FUNCTIONS(CTrajectoryLoopFunctions, "trajectory_loop_functions")