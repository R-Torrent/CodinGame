package Puzzles.ClassicPuzzle.Easy;

import java.util.*;

/*
 * Defibrillators
 * Puzzles > Classic Puzzle > Easy
 */

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
public class Defibrillators {

    public static void main(String[] args) {
        Scanner in = new Scanner(System.in);
        String LON = in.next();
        String LAT = in.next();
        Coordinate user = new Coordinate(LON, LAT);
        int N = in.nextInt();
        if (in.hasNextLine()) {
            in.nextLine();
        }

        HashMap<Double, String> defibrillators = new HashMap<>(N);
        for (int i = 0; i < N; i++) {
            String DEFIB = in.nextLine();
            String[] splits = DEFIB.split(";");
            defibrillators.put(user.distanceTo(new Coordinate(splits[4], splits[5])), splits[1]);
        }
        TreeMap<Double, String> sorted = new TreeMap<>(defibrillators);

        // Write an answer using System.out.println()
        // To debug: System.err.println("Debug messages...");

        System.out.println(sorted.firstEntry().getValue());
        in.close();
    }

    static class Coordinate {

        final double lon;
        final double lat;

        Coordinate(String lon, String lat) {
            this.lon = Double.parseDouble(lon.replace(",", ".")) * Math.PI / 180;
            this.lat = Double.parseDouble(lat.replace(",", ".")) * Math.PI / 180;
        }

        Double distanceTo(Coordinate c2) {
            final double rt = 6371; // Radius of the earth in km.
            double x = (c2.lon - this.lon) * Math.cos((this.lat + c2.lat) / 2);
            double y = c2.lat - this.lat;
            return Math.sqrt(x * x + y * y) * rt;
        }

    }

}