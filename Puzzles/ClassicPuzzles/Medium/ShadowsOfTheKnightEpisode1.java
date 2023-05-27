package Puzzles.ClassicPuzzles.Medium;

import java.util.*;

/*
 * Shadows of the Knight - Episode 1
 * Puzzles > Classic Puzzle > Medium
 */

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
public class ShadowsOfTheKnightEpisode1 {

    public static void main(String[] args) {
        Scanner in = new Scanner(System.in);
        int W = in.nextInt(); // width of the building.
        int H = in.nextInt(); // height of the building.
        int N = in.nextInt(); // maximum number of turns before game over.
        int X0 = in.nextInt();
        int Y0 = in.nextInt();

        Point x = new Point(X0, Y0);
        // xtl : top-left
        // xbr : bottom-right
        Point xtl = new Point(0, 0);
        Point xbr = new Point(W - 1, H - 1);
        // game loop
        while (true) {
            String bombDir = in.next(); // the direction of the bombs from batman's current location (U, UR, R, DR, D, DL, L or UL)
            BombDir bd = BombDir.findDir(bombDir);
            bd.reduceSearchBox(x, xtl, xbr);
            x.substituteWith(xtl.midwayToPoint(xbr));

            // Write an action using System.out.println()
            // To debug: System.err.println("Debug messages...");

            // the location of the next window Batman should jump to.
            System.out.println(x.x + " " + x.y);
        }
    }

    static class Point {

        int x;
        int y;

        Point(int x, int y) {
            this.x = x;
            this.y = y;
        }

        Point midwayToPoint(Point point) {
            return new Point((x + point.x) / 2, (y + point.y) / 2);
        }

        public void substituteWith(Point point) {
            x = point.x;
            y = point.y;
        }

    }

    @FunctionalInterface
    interface TriFunction<T, U, V, R> {

        R apply(T t, U u, V v);

    }

    static enum BombDir {

        UP        ("U",  (x0, xtl, xbr) -> new Point(x0.x, xtl.y),
                                 (x0, xtl, xbr) -> new Point(x0.x, x0.y - 1)),
        UPRIGHT   ("UR", (x0, xtl, xbr) -> new Point(x0.x + 1, xtl.y),
                                 (x0, xtl, xbr) -> new Point(xbr.x, x0.y - 1)),
        RIGHT     ("R",  (x0, xtl, xbr) -> new Point(x0.x + 1, x0.y),
                                 (x0, xtl, xbr) -> new Point(xbr.x, x0.y)),
        DOWNRIGHT ("DR", (x0, xtl, xbr) -> new Point(x0.x + 1, x0.y + 1),
                                 (x0, xtl, xbr) -> xbr),
        DOWN      ("D",  (x0, xtl, xbr) -> new Point(x0.x, x0.y + 1),
                                 (x0, xtl, xbr) -> new Point(x0.x, xbr.y)),
        DOWNLEFT  ("DL", (x0, xtl, xbr) -> new Point(xtl.x , x0.y + 1),
                                 (x0, xtl, xbr) -> new Point(x0.x - 1, xbr.y)),
        LEFT      ("L",  (x0, xtl, xbr) -> new Point(xtl.x, x0.y),
                                 (x0, xtl, xbr) -> new Point(x0.x - 1, x0.y)),
        UPLEFT    ("UL", (x0, xtl, xbr) -> xtl,
                                 (x0, xtl, xbr) -> new Point(x0.x - 1, x0.y - 1)),
        ERROR     ("ER", null, null);

        private final String bombDir;
        private final TriFunction<Point, Point, Point, Point> fxtl;
        private final TriFunction<Point, Point, Point, Point> fxbr;

        BombDir(String bombDir,
                TriFunction<Point, Point, Point, Point> fxtl,
                TriFunction<Point, Point, Point, Point> fxbr) {
            this.bombDir = bombDir;
            this.fxtl = fxtl;
            this.fxbr = fxbr;
        }

        static BombDir findDir(String bombDir) {
            for (BombDir bd : BombDir.values())
                if (bd.bombDir.equals(bombDir))
                    return bd;
            return BombDir.ERROR;
        }

        void reduceSearchBox(Point x, Point xtl, Point xbr) {
            xtl.substituteWith(fxtl.apply(x, xtl, xbr));
            xbr.substituteWith(fxbr.apply(x, xtl, xbr));
        }

    }

}