package Challenge.FallChallenge2024;

import java.util.*;

class Player {

    private final Scanner in;
    private int gameTurn;
    private final Intelligence ai;

    public static void main(String[] args) {
        Player player = new Player();
        try {
            player.runGame();
        } catch (Exception e) {
            e.printStackTrace(System.err);
            System.err.println("OOPS! Something went terribly wrong");
            player.deadColony();
        } finally {
            player.in.close();
        }
    }

    Player() {
        in = new Scanner(System.in);
        gameTurn = 1;
        ai = new Intelligence(this);
    }

    private void runGame() {
        while (true) {
            loadTurn();
            ai.think();
            System.out.println(ai.issueActions());
            gameTurn++;
        }
    }

    private void deadColony() {
        while (true) {
            loadTurn();
            System.out.println("WAIT");
            gameTurn++;
        }
    }

    private void loadTurn() {
        int resources = in.nextInt();
        int numTravelRoutes = in.nextInt();
        for (int i = 0; i < numTravelRoutes; i++) {
            int buildingId1 = in.nextInt();
            int buildingId2 = in.nextInt();
            int capacity = in.nextInt();
        }
        int numPods = in.nextInt();
        if (in.hasNextLine()) {
            in.nextLine();
        }
        for (int i = 0; i < numPods; i++) {
            String podProperties = in.nextLine();
        }
        int numNewBuildings = in.nextInt();
        if (in.hasNextLine()) {
            in.nextLine();
        }
        for (int i = 0; i < numNewBuildings; i++) {
            String buildingProperties = in.nextLine();
        }
    }
}

class Intelligence {

    private final Player player;

    Intelligence(Player player) {
        this.player = player;
    }

    void think() {
    }

    String issueActions() {
        return "";
    }

}

record Point(int coordX, int coordY) {}

record Building(int type, int id, Point point) {}

class TravelRoutes {

    private final Building building1;
    private final Building building2;
    private int capacity;

    TravelRoutes(Building building1, Building building2, int capacity) {
        this.building1 = building1;
        this.building2 = building2;
        this.capacity = capacity;
    }

}