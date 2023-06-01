package Challenge.SpringChallenge2023.Wood1;

import java.util.*;
import java.util.function.Predicate;

/*
 * Spring Challenge 2023
 * Contest
 */

/*
 * Wood 1 League
 * My swarm bee-lines for the eggs in a first stage, then forms into two columns of army ants ("marabuntas") as soon
 * as all the eggs have been collected:
 * First column (and main) forays the opposing colony's zone of influence.
 * Second column repeatedly targets the crystal source closest to base,
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
    private ArrayList<Cell> potentialResources;
    private int myPopulation;

    public static void main(String[] args) {
        Player player = new Player();
        try {
            player.runGame();
        }
        catch (Exception e) {
            e.printStackTrace(System.err);
            System.out.println(Command.MESSAGE.formCommand(null, null, 0, "OOPS! Brain dead"));
            player.deadBrain();
        }
        finally {
            player.in.close();
        }
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

    private void deadBrain() {
        while (true) {
            loadTurn();
            System.out.println(Command.WAIT.formCommand(null, null, 0, null));
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

        potentialResources = new ArrayList<>(numberOfCells);
        for (Cell c : map)
            if (c.getResource() != Resource.NONE && c.getQuantity() > 0)
                potentialResources.add(c);
        potentialResources.trimToSize();
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

        potentialResources.removeIf(c -> c.getQuantity() == 0);
    }

    int getGameTurn() { return gameTurn; }

    Cell[] getMap() { return map; }

    Cell[] getMyBases() { return myBases; }

    Cell[] getOppBases() { return oppBases; }

    ArrayList<Cell> getPotentialResources() { return potentialResources; }

    int getMyPopulation() { return myPopulation; }

}

class AntBrain {

    final Player player;
    static int[][] shortestPath;
    final Cell base0;
    List<Marabunta> marabuntas;
    List<Cell> availableTargets;

    AntBrain(Player player) {
        this.player = player;

        shortestPath = shortestPaths(player.getMap());

        base0 = player.getMyBases()[0];
        // targets order by proximity to base (first) and greater size (second)
        player.getPotentialResources().sort(comparatorDistancesTo(base0));
    }

    void musterColony() {
        marabuntas = new ArrayList<>(3);
        marabuntas.add(new Marabunta(0, Strategy.HARVESTFAR));
        marabuntas.add(new Marabunta(0, Strategy.HARVESTCLOSE));
        marabuntas.add(new Marabunta(0, Strategy.EGGSEAKER));
    }

    void think() {
        assignAnts();
        availableTargets = new ArrayList<>(player.getPotentialResources());
        marabuntas.stream()
                .filter(m -> m.getSize() != 0)
                .forEach(m -> availableTargets.remove(m.getTarget()));

        for (Marabunta m : marabuntas) {
            m.clearMemory();
            if (m.getSize() == 0) continue;
            if ((m.getTarget() == null || m.getTarget().getQuantity() == 0) && availableTargets.size() != 0)
                m.setTarget(availableTargets);
            int antDensity = m.getSize() / (shortestPath[base0.getIndex()][m.getTarget().getIndex()] + 1);
            m.addCommand(Command.LINE.formCommand(base0, m.getTarget(), antDensity, null));
        }
    }

    void assignAnts() {
        int ants = player.getMyPopulation();
        if (player.getPotentialResources().stream()
                .anyMatch(((Predicate<Cell>)c -> c.getResource().isType(Resource.EGGS))
                        .and(c -> shortestPath[base0.getIndex()][c.getIndex()] < ants))) {
            marabuntas.get(0).setSize(0);
            marabuntas.get(1).setSize(0);
            marabuntas.get(2).setSize(ants);
        }
        else {
            if (player.getPotentialResources().size() > 1) {
                int mara0 = ants * 6 / 10;
                int mara1 = ants * 4 / 10;
                marabuntas.get(0).setSize(mara0);
                marabuntas.get(1).setSize(mara1);
            }
            else {
                marabuntas.get(0).setSize(ants);
                marabuntas.get(1).setSize(0);
            }
            marabuntas.get(2).setSize(0);
        }
    }

    String issueCommands() {
        StringJoiner commands = new StringJoiner(";");

        if (player.getGameTurn() == 0)
            commands.add(Command.MESSAGE.formCommand(null, null, 0, "gl hf"));
        marabuntas.stream().map(Marabunta::getCommands).flatMap(List::stream).forEach(commands::add);

        return commands.toString();
    }

    static Cell findClosestTo(Cell to, List<Cell> pool) {
        if (to == null || pool == null) return null;
        List<Cell> copy = new ArrayList<>(pool);
        copy.remove(to);
        copy.sort(comparatorDistancesTo(to));

        return copy.get(0);
    }

    static private Comparator<Cell> comparatorDistancesTo(Cell to) {
        // targets order by proximity (first) and greater size (second)
        return (c1, c2) -> {
            int path = shortestPath[to.getIndex()][c1.getIndex()] - shortestPath[to.getIndex()][c2.getIndex()];
            return path != 0 ? path : c2.getQuantity() - c1.getQuantity();
        };
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
    final private Strategy strategy;
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

    List<String> getCommands() { return commands; }

    void addCommand(String command) {
        commands.add(command);
    }

    void clearMemory() {
        commands.clear();
    }

    Cell getTarget() { return target; }

    void setTarget(List<Cell> availableTargets) {
        target = strategy.selectNextTarget(target, availableTargets);
        availableTargets.remove(target);
    }

}

enum Strategy {

    HARVESTFAR {
        Cell selectNextTarget(Cell currentTarget, List<Cell> availableTargets) {
            return currentTarget == null
                    ? availableTargets.get(availableTargets.size() / 2)
                    : AntBrain.findClosestTo(currentTarget, availableTargets);
        } },
    HARVESTCLOSE {
        Cell selectNextTarget(Cell currentTarget, List<Cell> availableTargets) {
            return currentTarget == null
                    ? availableTargets.get(availableTargets.size() / 4)
                    : AntBrain.findClosestTo(currentTarget, availableTargets);
        } },
    EGGSEAKER {
        Cell selectNextTarget(Cell currentTarget, List<Cell> availableTargets) {
            return availableTargets.stream()
                    .filter(c -> c.getResource().isType(Resource.EGGS))
                    .findFirst()
                    .orElse(null);
        } };

    abstract Cell selectNextTarget(Cell currentTarget, List<Cell> availableTargets);

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

    int getIndex() { return index; }

    Resource getResource() { return resource; }

    int getQuantity() { return quantity; }

    void setQuantity(int quantity) { this.quantity = quantity; }

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

    int getMyAnts() { return myAnts; }

    void setMyAnts(int myAnts) { this.myAnts = myAnts; }

    int getOppAnts() { return oppAnts; }

    void setOppAnts(int oppAnts) { this.oppAnts = oppAnts; }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        Cell cell = (Cell) o;
        return index == cell.index;
    }

}

enum Resource {

    NONE    (0),
    EGGS    (1),
    CRYSTAL (2);

    final int type;

    Resource(int type) { this.type = type; }

    boolean isType(Resource resource) {
        return this.type == resource.type;
    }

    static Resource determineResource(int type) {
        switch (type) {
            case 1: return Resource.EGGS;
            case 2: return Resource.CRYSTAL;
            default: return Resource.NONE;
        }
    }

}

// WAIT | LINE <sourceIdx> <targetIdx> <strength> | BEACON <cellIdx> <strength> | MESSAGE <text>
enum Command {

    BEACON ("BEACON ") {
        StringJoiner construct(StringJoiner sj, Cell cell1, Cell cell2, int strength, String text) {
            return sj.add(Integer.toString(cell2.getIndex()))
                     .add(Integer.toString(strength));
        }; },
    LINE ("LINE ") {
        StringJoiner construct(StringJoiner sj, Cell cell1, Cell cell2, int strength, String text) {
            return sj.add(Integer.toString(cell1.getIndex()))
                     .add(Integer.toString(cell2.getIndex()))
                     .add(Integer.toString(strength));
        }; },
    WAIT ("WAIT") {
        StringJoiner construct(StringJoiner sj, Cell cell1, Cell cell2, int strength, String text) {
            return sj;
        }; },
    MESSAGE ("MESSAGE ") {
        StringJoiner construct(StringJoiner sj, Cell cell1, Cell cell2, int strength, String text) {
            return sj.add(text);
        }; };

    private final String command;

    Command(String command) { this.command = command; }

    String formCommand(Cell cell1, Cell cell2, int strength, String text) {
        return construct(new StringJoiner(" ", command, ""), cell1, cell2, strength, text).toString();
    }

    abstract StringJoiner construct(StringJoiner sj, Cell cell1, Cell cell2, int strength, String text);

}