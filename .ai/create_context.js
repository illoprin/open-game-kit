import { readdir, readFile, writeFile } from "node:fs/promises";
import path from "node:path";

const srcDir = path.resolve(process.argv[2] ?? "");
const templateFile = path.resolve(process.argv[3] ?? "");
const outputFile = path.resolve(process.argv[4] ?? "context.md");

if (!process.argv[2] || !process.argv[3]) {
  console.error(
    "Usage: node generate-context.mjs <src-directory> <template.md> [output.md]",
  );
  process.exit(1);
}

async function collectHeaders(dir) {
  const entries = await readdir(dir, { withFileTypes: true });
  const files = [];

  for (const entry of entries) {
    const fullPath = path.join(dir, entry.name);

    if (entry.isDirectory()) {
      files.push(...(await collectHeaders(fullPath)));
    } else if (entry.isFile() && /\.(h|hpp)$/i.test(entry.name)) {
      files.push(fullPath);
    }
  }

  return files.sort();
}

function makeTree(rootName, relativePaths) {
  const tree = new Map();

  for (const relativePath of relativePaths) {
    let current = tree;

    for (const part of relativePath.split(path.sep)) {
      if (!current.has(part)) current.set(part, new Map());
      current = current.get(part);
    }
  }

  function render(node, prefix = "") {
    return [...node.entries()]
      .map(([name, child], index, entries) => {
        const last = index === entries.length - 1;
        const branch = last ? "└── " : "├── ";
        const nextPrefix = prefix + (last ? "    " : "│   ");
        const children = child.size ? `\n${render(child, nextPrefix)}` : "";
        return `${prefix}${branch}${name}${children}`;
      })
      .join("\n");
  }

  return `${rootName}\n${render(tree)}`;
}

const headers = await collectHeaders(srcDir);
const relativePaths = headers.map((file) => path.relative(srcDir, file));

const fileSections = await Promise.all(
  headers.map(async (file, index) => {
    const relativePath = relativePaths[index].split(path.sep).join("/");
    const content = await readFile(file, "utf8");
    return `### ${relativePath}\n\n\`\`\`\n${content.replace(/\n?$/, "\n")}\`\`\``;
  }),
);

let markdown = await readFile(templateFile, "utf8");
markdown = markdown
  .replace("{{STRUCTURE}}", makeTree(path.basename(srcDir), relativePaths))
  .replace("{{FILES}}", fileSections.join("\n\n"));

await writeFile(outputFile, markdown, "utf8");
console.log(`Generated ${outputFile}`);
