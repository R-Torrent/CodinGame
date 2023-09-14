package Challenge.SummerChallenge2023.HiringNewTalent;

import java.util.*;
import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.google.gson.reflect.TypeToken;

public class Question3 {

    /**
     * @param fileContents A list of strings, where each string represents the contents of a file.
     * @return The contents of the merged file.
     */
    public static String mergeFiles(List<String> fileContents) {
        // Write your code here
        Map<String, Set<Field>> db = new TreeMap<>(Comparator.naturalOrder());
        for (String file : fileContents) {
            String[] entries = file.split("\\n");
            for (String entry : entries) {
                String entryName = "";
                Set<Field> setEntry = new TreeSet<>(Comparator.comparing(a -> a.name));
                String[] fields = entry.split(";");
                for (String field : fields) {
                    String[] comp = field.split("=");
                    if (comp[0].equals("Name"))
                        entryName = comp[1];
                    else
                        setEntry.add(new Field(comp[0], comp[1]));
                }
                Set<Field> values = db.get(entryName);
                if (values != null)
                    values.addAll(setEntry);
                else
                    db.put(entryName, setEntry);
            }
        }

        StringJoiner output = new StringJoiner("\n");
        for (Map.Entry<String, Set<Field>> mapEntry : db.entrySet()) {
            StringJoiner outputLine = new StringJoiner(";");
            outputLine.add("Name=" + mapEntry.getKey());
            for (Field outputField : mapEntry.getValue())
                outputLine.add((outputField.name + "=" + outputField.content));
            output.add(outputLine.toString());
        }

        return output.toString();
    }

    record Field (String name, String content) {}

    /* Ignore and do not change the code below */
    private static final Gson gson = new GsonBuilder().disableHtmlEscaping().create();

    /**
     * Try a solution
     * @param mergedFile The contents of the merged file.
     */
    public static void trySolution(String mergedFile) {
        System.out.println("" + gson.toJson(mergedFile));
    }

    public static void main(String args[]) {
        try (Scanner in = new Scanner(System.in)) {
            trySolution(mergeFiles(
                    gson.fromJson(in.nextLine(), new TypeToken<List<String>>(){}.getType())
            ));
        }
    }
    /* Ignore and do not change the code above */

}