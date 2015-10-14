import javax.swing.*;
import java.util.Arrays;

public class RouterNode {
  private int myID;
  private GuiTextArea myGUI;
  private RouterSimulator sim;
  private boolean poison = true;

  private int[][] distanceTable = new int[RouterSimulator.NUM_NODES][RouterSimulator.NUM_NODES];
  private int[] costs = new int[RouterSimulator.NUM_NODES];
  private int[] travelCosts = new int[RouterSimulator.NUM_NODES]; // only for printing.
  private int[] route = new int[RouterSimulator.NUM_NODES];

  //--------------------------------------------------
  public RouterNode(int ID, RouterSimulator sim, int[] costs) {
    myID = ID;
    this.sim = sim;
    myGUI =new GuiTextArea("  Output window for Router #"+ ID + "  ");
    System.arraycopy(costs, 0, this.costs, 0, RouterSimulator.NUM_NODES);
    System.arraycopy(costs, 0, this.travelCosts, 0, RouterSimulator.NUM_NODES);
    initDistanceTable();

    printDistanceTable();
  }

  private void initDistanceTable(){
    //fill distanceTable with inf.
    for (int[] row: distanceTable)
    Arrays.fill(row, sim.INFINITY);
    //Set table of our own ID since we know our outward costs
    distanceTable[myID] = Arrays.copyOf(costs, costs.length);

    //populate the routes (-1 for no route known.)
    for (int i = 0; i < sim.NUM_NODES ; i++ ) {
      if (costs[i] != sim.INFINITY){
        route[i] = i;
      }else{
        route[i] = -1;
      }
    }

    updateAll();

  }

  //--------------------------------------------------
  public void recvUpdate(RouterPacket pkt) {
    boolean changed = false;
    //add their vector to the distanceTable
    distanceTable[pkt.sourceid] = pkt.mincost;

    changed = updateTable();

    if(changed){
      updateAll();
    }
  }

  //--------------------------------------------------
  private void updateAll(){
    //send our best cost vector to the neighbours
    for (int nbr = 0; nbr < sim.NUM_NODES ; nbr++ ) {
      if(nbr == myID || costs[nbr] == sim.INFINITY)
      continue;
      //don't care that we send towards non-neighbours and self. it is taken care of in the emulator.
      int[] myCosts = Arrays.copyOf(distanceTable[myID],sim.NUM_NODES);
      if(poison){
        for (int dest = 0; dest < RouterSimulator.NUM_NODES; dest++) {
          // Poisoned reverse
          //if destination goes through neighbour and the neighbour isn't the destination
          if (route[dest] == nbr && dest != nbr) {
            myCosts[dest] = RouterSimulator.INFINITY;
          }
        }

      }
      RouterPacket pkt = new RouterPacket(myID,nbr,myCosts);
      sendUpdate(pkt);
    }
  }

  //--------------------------------------------------
  private void sendUpdate(RouterPacket pkt) {
    sim.toLayer2(pkt);
  }




  //--------------------------------------------------
  public void updateLinkCost(int dest, int newcost) {
    costs[dest] = newcost;
    updateTable();
    updateAll();
  }


  private boolean updateTable(){
    boolean updated = false;

    int bestRoute = -1;
    int bestCost = -1;
    //for all destinations except self
    for (int dest = 0; dest < sim.NUM_NODES; ++dest) {
      if (dest == myID)
      continue;

      bestCost = sim.INFINITY;

      //for all other routers except myself
      for (int node = 0; node < sim.NUM_NODES; ++node){
        if (node == myID)
        continue;

        //calculate cost for this path [dest] through [node]
        int currentCost = costs[node] + distanceTable[node][dest];
        //if cost is better than the current best cost update the estimate and the route.
        if (bestCost > currentCost) {
          bestCost = currentCost;
          bestRoute = node;
        }

      }
      //updated set to true if the cost has changed or if the route has changed.
      if (!updated)
      updated = distanceTable[myID][dest] != bestCost || bestRoute != route[dest];

      //place the new cost and route in the table
      distanceTable[myID][dest] = bestCost;
      travelCosts[dest] = bestCost;
      route[dest] = bestRoute;
    }

    return updated;

  }

  //--------------------------------------------------
  public void printDistanceTable() {

    myGUI.println("");
    myGUI.println("");
    myGUI.println("");
    myGUI.println("");
    myGUI.println("Current table for " + myID +
    "  at time " + sim.getClocktime());
    myGUI.println("");
    myGUI.println("");
    String temp = F.format("dst",10) + "|";
    myGUI.println("Distancetable:");

    for (int i = 0; i < sim.NUM_NODES ; ++i ) {
      temp += F.format(i,10);
    }
    myGUI.println(temp);
    myGUI.println("-----------------------------");

    for (int i = 0 ; i < sim.NUM_NODES ; ++i ) {
      if (i == myID || costs[i] == sim.INFINITY)
      continue;
      String temp2 = F.format("nbr  " + i, 10) + "|";
      for (int j = 0; j < sim.NUM_NODES; ++j ) {
        temp2 += F.format(distanceTable[i][j],10);
      }
      myGUI.println(temp2);
    }

    myGUI.println("");
    myGUI.println("Our distance vector and routes");
    myGUI.println(temp);
    myGUI.println("-----------------------------");

    String temp3 = F.format("cost",10) + "|";
    String temp4 = F.format("route",10) + "|";
    for (int j = 0; j < sim.NUM_NODES; ++j ) {
      temp3 += F.format(travelCosts[j],10);
      temp4 += F.format(route[j],10);
    }
    myGUI.println(temp3);
    myGUI.println(temp4);
  }

}
