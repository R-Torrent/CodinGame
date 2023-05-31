package Challenge.SpringChallenge2023.Wood2;

import java.util.*;

/*
 * Spring Challenge 2023
 * Contest
 */

/*
 * Wood 2 League
 * My swarm forms two columns of army ants ("marabuntas"):
 * First column forays the opposing colony's zone of influence.
 * Second (and main) column repeatedly targets the crystal source closest to base,
 *   and exhausts it before moving-on to the next gathering area.
 */

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
class Player {

    private final Scanner in;
    private int gameTurn;
    private final AntBrain overmind;
    private Cell[] map, myBases, oppBases;
    private ArrayList<Cell> activeCrystalSources;
    private int myPopulation;

    public static void main(String[] args) {
        Player player = new Player();
        player.runGame();
        player.in.close();
    }

    Player() {
        in = new Scanner(System.in);
        gameTurn = 0;
        loadField();
        overmind = new AntBrain(this);
    }

    private void runGame() {
        overmind.musterColony();

        // game loop
        while (true) {
            loadTurn();
            overmind.think();

            // Write an action using System.out.println()
            // To debug: System.err.println("Debug messages...");
            System.out.println(overmind.issueCommands());

            gameTurn++;
        }
    }

    private void loadField() {
        int numberOfCells = in.nextInt(); // amount of hexagonal cells in this map
        map = new Cell[numberOfCells];

        int[][] neighbors = new int[numberOfCells][6];
        for (int i = 0; i < numberOfCells; i++) {
            int type = in.nextInt(); // 0 for empty, 1 for eggs, 2 for crystal
            int initialResources = in.nextInt(); // the initial amount of eggs/crystals on this cell
            int neigh0 = in.nextInt(); // the index of the neighbouring cell for each direction
            int neigh1 = in.nextInt();
            int neigh2 = in.nextInt();
            int neigh3 = in.nextInt();
            int neigh4 = in.nextInt();
            int neigh5 = in.nextInt();
            neighbors[i] = new int[]{ neigh0, neigh1, neigh2, neigh3, neigh4, neigh5 };
            map[i] = new Cell(i, Resource.determineResource(type), initialResources);
        }
        for (Cell c : map)
            c.setNeighbors(neighbors[c.getIndex()], map);

        int numberOfBases = in.nextInt();
        myBases = new Cell[numberOfBases];
        for (int i = 0; i < numberOfBases; i++) {
            int myBaseIndex = in.nextInt();
            myBases[i] = map[myBaseIndex];
        }
        oppBases = new Cell[numberOfBases];
        for (int i = 0; i < numberOfBases; i++) {
            int oppBaseIndex = in.nextInt();
            oppBases[i] = map[oppBaseIndex];
        }

        activeCrystalSources = new ArrayList<>(numberOfCells);
        for (Cell c : map)
            if (c.getResource() == Resource.CRYSTAL && c.getQuantity() > 0)
                activeCrystalSources.add(c);
        activeCrystalSources.trimToSize();
    }

    private void loadTurn() {
        myPopulation = 0;
        for (Cell c : map) {
            int resources = in.nextInt(); // the current amount of eggs/crystals on this cell
            int myAnts = in.nextInt(); // the amount of your ants on this cell
            int oppAnts = in.nextInt(); // the amount of opponent ants on this cell
            c.setQuantity(resources);
            c.setMyAnts(myAnts);
            c.setOppAnts(oppAnts);
            myPopulation += myAnts;
        }

        activeCrystalSources.removeIf(c -> c.getQuantity() == 0);
    }

    int getGameTurn() { return gameTurn; }

    Cell[] getMap() { return map; }

    Cell[] getMyBases() { return myBases; }

    Cell[] getOppBases() { return oppBases; }

    ArrayList<Cell> getActiveCrystalSources() { return activeCrystalSources; }

    int getMyPopulation() { return myPopulation; }

}

class AntBrain {

    final Player player;
    int[][] shortestPath;
    List<Marabunta> marabuntas;

    AntBrain(Player player) {
        this.player = player;
        shortestPath = shortestPaths(player.getMap());
        int base0 = player.getMyBases()[0].getIndex();
        player.getActiveCrystalSources().sort(
                (c1, c2) -> shortestPath[base0][c1.getIndex()] - shortestPath[base0][c2.getIndex()]);
    }

    void musterColony() {
        marabuntas = new ArrayList<>(2);
        marabuntas.add(new Marabunta(0, Strategy.STEALMINERALS));
        marabuntas.add(new Marabunta(0, Strategy.CLOSESTFROMBASE));
    }

    void think() {
        if (player.getGameTurn() == 0) {
            int sizeMara0 = player.getMyPopulation() * 4 / 10;
            int sizeMara1 = player.getMyPopulation() * 6 / 10;
            marabuntas.get(0).setSize(sizeMara0);
            marabuntas.get(1).setSize(sizeMara1);
        }
        else
            marabuntas.forEach(Marabunta::clearMemory);

        int crystalPatches = player.getActiveCrystalSources().size();
        if (crystalPatches == 1 && marabuntas.size() == 2) {
            marabuntas.remove(0); // disband raiders
            marabuntas.get(0).setSize(player.getMyPopulation());
        }

        for (Marabunta m : marabuntas) {
            if (m.getTarget() == null || m.getTarget().getQuantity() == 0)
                switch (m.getStrategy()) {
                    case CLOSESTFROMBASE:
                        m.setTarget(player.getActiveCrystalSources().get(0));
                        break;
                    case STEALMINERALS:
                        Cell nextTarget = player.getActiveCrystalSources().get(crystalPatches - 1);
                        if (player.getGameTurn() == 0)
                            nextTarget = player.getActiveCrystalSources().get(crystalPatches / 2);
                        else if (player.getActiveCrystalSources().size() > 2) {
                            int minPath = player.getMap().length, path;
                            for (Cell candidate : player.getActiveCrystalSources()) {
                                if (candidate == m.getTarget()) continue;
                                if ((path = shortestPath[m.getTarget().getIndex()][candidate.getIndex()]) < minPath) {
                                    nextTarget = candidate;
                                    minPath = path;
                                }
                            }
                        }
                        m.setTarget(nextTarget);
                    default:
                        break;
                }
            Cell base0 = player.getMyBases()[0];
            int antDensity = m.getSize() / (shortestPath[base0.getIndex()][m.getTarget().getIndex()] + 1);
            m.addCommand(singleCommand(Command.LINE, base0, m.getTarget(), antDensity, null));
        }
    }

    String issueCommands() {
        StringJoiner commands = new StringJoiner(";");

        if (player.getGameTurn() == 0)
            commands.add(singleCommand(Command.MESSAGE, null, null, 0, "gl hf"));
        marabuntas.stream().map(Marabunta::getCommands).flatMap(List::stream).forEach(commands::add);

        return commands.toString();
    }

    // WAIT | LINE <sourceIdx> <targetIdx> <strength> | BEACON <cellIdx> <strength> | MESSAGE <text>
    static private String singleCommand(Command command, Cell cell1, Cell cell2, int strength, String text) {
        StringJoiner sc = new StringJoiner(" ", command.getCommand(), "");

        switch (command) {
            case LINE:
                sc.add(Integer.toString(cell1.getIndex()));
            case BEACON:
                sc.add(Integer.toString(cell2.getIndex())).add(Integer.toString(strength));
                break;
            case MESSAGE:
                sc.add(text);
            case WAIT: default:
                break;
        }

        return sc.toString();
    }

    // Floyd-Warshall algorithm
    static private int[][] shortestPaths(Cell[] map) {
        final int N = map.length;
        int[][] shortestPath = new int[N][N];

        for (int i = 0; i < N; i++) {
            shortestPath[i][i] = 0;
            for (int j = i + 1; j < N; j++)
                shortestPath[j][i] = shortestPath[i][j] = map[i].isNeighborWith(j) ? 1 : N;
        }
        for (int k = 0; k < N; k++)
            for (int i = 0; i < N; i++)
                for (int j = i + 1; j < N; j++) {
                    int path = shortestPath[i][k] + shortestPath[k][j];
                    if (shortestPath[i][j] > path)
                        shortestPath[j][i] = shortestPath[i][j] = path;
                }

        return shortestPath;
    }

}

class Marabunta {

    private int size;
    private Strategy strategy;
    final private List<String> commands;
    private Cell target;

    Marabunta(int size, Strategy strategy)
    {
        this.size = size;
        this.strategy = strategy;
        commands = new ArrayList<>();
    }

    int getSize() { return size; }

    void setSize(int size) { this.size = size; }

    Strategy getStrategy() { return strategy; }

    void setStrategy(Strategy strategy) { this.strategy = strategy; }

    List<String> getCommands() { return commands; }

    void addCommand(String command) {
        commands.add(command);
    }

    void clearMemory() {
        commands.clear();
    }

    Cell getTarget() { return target; }

    void setTarget(Cell target) { this.target = target; }

}

class Cell {

    private final int index;
    private final Resource resource;
    private int quantity;
    private Cell[] neighbors;
    private int myAnts;
    private int oppAnts;

    Cell(int index, Resource resource, int quantity) {
        this.index = index;
        this.resource = resource;
        this.quantity = quantity;
    }

    int getIndex() {
        return index;
    }

    Resource getResource() {
        return resource;
    }

    int getQuantity() {
        return quantity;
    }

    void setQuantity(int quantity) {
        this.quantity = quantity;
    }

    boolean isNeighborWith(int indexN) {
        for (Cell c : neighbors)
            if (c != null && c.index == indexN)
                return true;
        return false;
    }

    void setNeighbors(int[] indices, Cell[] map) {
        neighbors = new Cell[6];
        for (int i = 0; i < 6; i++)
            neighbors[i] = indices[i] >= 0 ? map[indices[i]] : null;
    }

    int getMyAnts() {
        return myAnts;
    }

    void setMyAnts(int myAnts) {
        this.myAnts = myAnts;
    }

    int getOppAnts() {
        return oppAnts;
    }

    void setOppAnts(int oppAnts) {
        this.oppAnts = oppAnts;
    }

}

enum Resource {

    NONE    (0),
    EGGS    (1),
    CRYSTAL (2);

    final int type;

    Resource(int type) { this.type = type; }

    static Resource determineResource (int type) {
        switch (type) {
            case 1: return Resource.EGGS;
            case 2: return Resource.CRYSTAL;
            default: return Resource.NONE;
        }
    }

}

enum Command {

    BEACON  ("BEACON "),
    LINE    ("LINE "),
    WAIT    ("WAIT"),
    MESSAGE ("MESSAGE ");

    private final String command;

    Command(String command) { this.command = command; }

    String getCommand() { return command; }

}

enum Strategy {

    CLOSESTFROMBASE,
    STEALMINERALS

}