#ifndef BOX_TRAJECTORY_LOOP_FUNCTIONS_H
#define BOX_TRAJECTORY_LOOP_FUNCTIONS_H

#include <argos3/core/simulator/loop_functions.h>
#include <buzz/argos/buzz_loop_functions.h>
#include <argos3/plugins/simulator/entities/box_entity.h>

using namespace argos;

class CBoxTrajectoryLoopFunctions : public CBuzzLoopFunctions {

public:

   typedef std::map<CBoxEntity*, std::vector<CVector3> > TWaypointMap;
   TWaypointMap m_tWaypoints;
   
public:

   CBoxTrajectoryLoopFunctions() {}
   virtual ~CBoxTrajectoryLoopFunctions() {}

   virtual void Init(TConfigurationNode& t_tree);

   virtual void Reset();

   virtual void PostStep();

   inline const TWaypointMap& GetWaypoints() const {
      return m_tWaypoints;
   }

   virtual void BuzzBytecodeUpdated();

private:

  /** The stimuli associated to the tasks */
   std::vector<CVector3> m_vecStimuli;
};

#endif
